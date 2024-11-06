#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "quickloader.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/font.hpp"
#include "solosnake/fontprinter.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/img.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/make_font.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/utf8text.hpp"

// BLUEDATADIR is defined by CMake.

using namespace solosnake;

#define WINDOW_W 1024
#define WINDOW_H 768
// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (10 * 20)

namespace
{
    std::shared_ptr<deferred_renderer> g_rr;
    std::shared_ptr<fontprinter> g_printer;
    std::shared_ptr<imesh> g_msrc;
    std::string g_fontname = "basic24.xml";
    meshhandle_t g_h = 0;
    texturehandle_t g_t1 = 0;
    bool g_stop = false;
    size_t g_loopcounter = 0;
    unsigned int g_window_width = WINDOW_W;
    unsigned int g_window_height = WINDOW_H;
    matrix4x4_t g_mv;
    matrix4x4_t g_proj;
    const float g_zNear = 0.1f;

    //-------------------------------------------------------------------------

    void create_quad_tex()
    {
        const size_t w = 128;
        const size_t g_h = 128;

        solosnake::image img1( w, g_h, solosnake::image::format_bgra );

        solosnake::image::clear_layer_to( img1, 0, 0xFF );
        solosnake::image::clear_layer_to( img1, 1, 0x00 );
        solosnake::image::clear_layer_to( img1, 2, 0x00 );
        solosnake::image::clear_layer_to( img1, 3, 0x40 );

        img1.save( BLUEDATADIR "/textures/quad2.bmp" );
    }

    void create_dynbuffer_tex()
    {
        const size_t w = 128;
        const size_t g_h = 128;

        solosnake::image img1( w, g_h, solosnake::image::format_bgra );

        solosnake::image::clear_layer_to( img1, 0, 0x0F );
        solosnake::image::clear_layer_to( img1, 1, 0x00 );
        solosnake::image::clear_layer_to( img1, 2, 0xF0 );
        solosnake::image::clear_layer_to( img1, 3, 0x40 );

        img1.save( BLUEDATADIR "/meshes/dynbuffertex2.bmp" );
    }

    void init( const rect& screenRect )
    {
        // const unsigned char material = 128;
        // generate_model_normal_tex( material );

        std::vector<std::string> directories;
        directories.push_back( BLUEDATADIR "/meshes" );
        directories.push_back( BLUEDATADIR "/textures" );

        auto assetsDir = std::make_shared<datadirectories>( directories );

        auto url = assetsDir->get_file( "thruster.msh" );
        auto meshes = load_mesh_datas( url, assetsDir );
        ASSERT_TRUE( meshes.size() == 1 );

        g_msrc = meshes.at( 0 );

        g_rr = std::make_shared<deferred_renderer>(
                   screenRect.width(), screenRect.height(), deferred_renderer::HighQuality ); // 1920 x 1080

        g_h = g_rr->load_mesh( g_msrc );

        // Quads
        {
            create_quad_tex();
            auto tex1 = assetsDir->get_file( "quad2.bmp" );
            g_t1 = g_rr->load_texture( std::make_shared<solosnake::img::src>( tex1 ) );
        }

        // Font g_printer
        {
            datadirectory fontfiles( BLUEDATADIR "/fonts" );

            auto fontURL   = fontfiles.get_file( g_fontname );
            auto texDir    = std::make_shared<datadirectory>( BLUEDATADIR "/textures" );
            auto images    = std::make_shared<img_cache>( texDir );
            auto texloader = std::make_shared<solosnake_test::quickloader>( g_rr, images );
            auto fnt       = std::make_shared<font>( fontURL, *texloader );

            g_printer = std::make_shared<fontprinter>( dimension2d<unsigned int>( WINDOW_W, WINDOW_H ), g_rr, fnt );
        }
    }

    // Converts a float in the range -1 to +1 to range 0 to 1
    float range_to_0_1( float f )
    {
        assert( ( ( f * 0.5f ) + 0.5f ) <= 1.0f );
        assert( ( ( f * 0.5f ) + 0.5f ) >= 0.0f );
        return ( f * 0.5f ) + 0.5f;
    }

    // Display 1 stationary cube and 2 nearby rotating cubes.
    void displayFn()
    {
        static float cameraDistance = 10.0f;
        static float cameraY = 2.50f;
        static float change = -0.01f;

        static float varier = -1.0f;
        static float dv = 0.01f;

        varier += dv;

        if( varier < -1.0f )
        {
            dv = 0.001f;
            varier = -1.0f;
        }
        else if( varier > 1.0f )
        {
            dv = -0.001f;
            varier = 1.0f;
        }

        static const size_t grid = 5;
        static const size_t N = grid * grid;

        const float spacing = 2.50f;

        cameraDistance += change;

        if( cameraDistance < ( 2 ) || cameraDistance > ( grid + grid * spacing ) )
        {
            change = -change;
        }

        // Model view - "camera".
        load_translation_4x4( 0.0f, -cameraY, -cameraDistance, g_mv );

        // 9 cubes of radius 2, in a grid, spaced 4 units apart, hard-coded.

        // Our cube spans from 1,1,1 to -1,-1,-1. Find the sphere that fits it
        // perfectly:

        deferred_renderer::point_light light[N];
        meshinstancedata cubes[N];

        const float colours[N][3] = { { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
        };

        float start = grid * spacing * 0.5f;

        // Initialise model instance data:
        for( size_t i = 0; i < grid; ++i )
        {
            for( size_t j = 0; j < grid; ++j )
            {
                auto ij = grid * j + i;

                load_translation_4x4(
                    -start + i * spacing, 0.0f, -start + j * spacing, cubes[ij].location_4x4() );

                cubes[ij].set_team_hue_shift( static_cast<float>( i ) / grid );

                light[ij].worldX = -start + i * spacing;
                light[ij].worldY = 0.0f + varier * 2.0f;
                light[ij].worldZ = -start + j * spacing;

                light[ij].radius = 2.0f;

                if( i == 1 )
                {
                    // light[ij].radius *= 1;
                }

                light[ij].red = colours[ij][0];
                light[ij].green = colours[ij][1];
                light[ij].blue = colours[ij][2];
            }
        }

        g_rr->set_proj_matrix( g_proj, g_zNear );
        g_rr->set_view_matrix( g_mv );
        g_rr->set_ambiant_light( 0.25f, 0.25f, 0.25f );

        g_rr->start_scene();
        g_rr->draw_mesh_instances( g_h, cubes, N );

        for( size_t i = 0; i < N; ++i )
        {
            if( i != 4 && i != 5 )
            {
                g_rr->draw_point_light( light[i] );
            }
        }

        const dimension2d<unsigned int> windowSize( g_window_width, g_window_height );

        g_printer->print_text( utf8text( "To be, or not to be, that is the question:"
                                         "Whether 'tis Nobler in the mind to suffer "
                                         "The Slings and Arrows of outrageous Fortune, "
                                         "Or to take Arms against a Sea of troubles, "
                                         "And by opposing end them: to die, to sleep "
                                         "No more; and by a sleep, to say we end "
                                         "The heart-ache, and the thousand Natural shocks "
                                         "That Flesh is heir to? 'Tis a consummation "
                                         "Devoutly to be wished. To die to sleep, "
                                         "To sleep, perchance to Dream; Ay, there's the rub, "
                                         "For in that sleep of death, what dreams may come, "
                                         "When we have shuffled off this mortal coil, "
                                         "Must give us pause. " ),
                               position2df( 0.5f, 0.0f ),
                               dimension2df(),
                               HorizontalAlignCentre,
                               VerticalAlignTop );

        // Finished:
        g_rr->end_scene();

        if( ++g_loopcounter == MAX_TEST_LOOPS )
        {
            g_stop = true;
        }
    }

    void reshapeFn( int w, int h )
    {
        g_window_height = h;
        g_window_width = static_cast<unsigned int>( w );
        load_perspective(
            solosnake::radians( PI_RADIANS * 0.25f ), float( w ) / g_h, g_zNear, 100.0f, g_proj );
        g_printer->set_screen_size(
            dimension2d<unsigned int>( static_cast<unsigned int>( w ), static_cast<unsigned int>( h ) ) );
        g_rr->set_viewport( 0, 0, w, h );
    }

    void exitFn( int )
    {
        g_msrc.reset();
        g_printer.reset();
        g_rr.reset();
    }
}

TEST( fontprinter, static_text )
{
    try
    {
        auto glut = glutWindow::make_shared( 3, 3, WINDOW_W, WINDOW_H );
        EXPECT_TRUE( GL_NO_ERROR == glGetError() );

        auto displayLoop = [&]()
        {
            displayFn();
            if( g_stop )
            {
                glut->end_display_loop();
            }
        };

        glut->glutDisplayFunc( displayLoop );
        glut->glutExitFunc( exitFn );
        glut->glutReshapeFunc( reshapeFn );

        init( glut->get_screen_rect() );

        glut->glutMainLoop();
    }
    catch( const std::exception& e )
    {
        ADD_FAILURE(e.what());
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }

    exitFn( 0 );
}

#endif
