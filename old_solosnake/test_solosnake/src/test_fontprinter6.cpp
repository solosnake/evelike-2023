#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <numeric>
#include <set>
#include "quickloader.hpp"
#include "solosnake/testing/testing.hpp"
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

using namespace solosnake;

// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (10 * 20)
#define WINDOW_W (1024)
#define WINDOW_H (768)
#define TESTFONT_NAME BLUEDATADIR "/fonts/FSEX300.ttf"
#define FONTLETTERS_NAME "fontprinter6test"
#define FONTSIZE (32)

namespace
{
    const solosnake::bgra red( 0x00, 0x00, 0xFF, 0xFF );
    const solosnake::bgra green( 0x00, 0xFF, 0x00, 0xFF );
    const solosnake::bgra blue( 0xFF, 0x00, 0x00, 0xFF );
    const solosnake::bgra orange( 0x00, 0x80, 0xFF, 0xFF );

    std::shared_ptr<deferred_renderer> g_rr;
    std::shared_ptr<fontprinter> g_printer;
    meshhandle_t g_h = 0;
    texturehandle_t g_t2 = 0;
    bool g_stop = false;
    size_t g_loopcounter = 0;
    unsigned int g_window_width = WINDOW_W;
    unsigned int g_window_height = WINDOW_H;
    std::shared_ptr<imesh> g_msrc;
    matrix4x4_t g_mv;
    matrix4x4_t g_proj;
    const float g_zNear = 0.1f;

    const char g_quote[] = "To be,\nor not to be,\n that is the question: Whether "
                           "'tis Nobler in the mind to "
                           "suffer The Slings and Arrows of outrageous Fortune, Or "
                           "to take Arms against a Sea of troubles, "
                           "And by opposing end them: to die, to sleep "
                           "No more; and by a sleep, to say we end "
                           "The heart-ache, and the thousand Natural shocks "
                           "That Flesh is heir to? 'Tis a consummation "
                           "Devoutly to be wished. To die to sleep, "
                           "To sleep, perchance to Dream; Ay, there's the rub, "
                           "For in that sleep of death, what dreams may come, "
                           "When we have shuffled off this mortal coil, "
                           "Must give us pause. ";

    //! Returns a sorted vector of the printable ascii characters.
    std::vector<std::uint32_t> ascii()
    {
        std::vector<std::uint32_t> printables( 1 + 0x7E - 0x20 );
        std::iota( printables.begin(), printables.end(), 0x20 );
        return printables;
    }

    //! Returns a sorted vector of the printable ascii characters.
    std::vector<std::uint32_t> tobeornottobe_letters()
    {
        std::set<std::uint32_t> printables;

        std::copy( 
            g_quote, 
            g_quote + sizeof( g_quote ), 
            std::inserter( printables, printables.begin() ) );

        return std::vector<std::uint32_t>( printables.begin(), printables.end() );
    }

    std::pair<std::shared_ptr<image>, std::shared_ptr<fontletters>>
            make_test_font( const std::vector<std::uint32_t>& chars )
    {
        unsigned int padding = 1;
        bool verbose = false;

        auto fnt = make_font( TESTFONT_NAME, FONTLETTERS_NAME ".bmp", chars, FONTSIZE, padding, verbose );

        fnt.first->save( fnt.second->texture_filename().c_str() );

        std::ofstream ofile( FONTLETTERS_NAME ".xml" );
        fnt.second->save( ofile );

        return fnt;
    }

    //-------------------------------------------------------------------------

    image create_screensized_image( unsigned int w, unsigned int h )
    {
        image img1( w, h, solosnake::image::format_bgra );
        img1.clear_to_bgra( 0xFF, 0xFF, 0xFF, 0xFF );

        for( unsigned int y = 0; y < h; ++y )
        {
            for( unsigned int x = 0; x < w; ++x )
            {
                if( ( x + y ) % 2 == 0 )
                {
                    *reinterpret_cast<bgra*>( image::pixel( x, y, img1, image::format_bgra ) ) = red;
                }
            }
        }

        return img1;
    }

    std::shared_ptr<fontprinter> make_test_fontprinter( std::shared_ptr<deferred_renderer> rr )
    {
        ss_log( "Making test font..." );
        make_test_font( tobeornottobe_letters() );
        ss_log( "...done." );

        auto dir = std::make_shared<datadirectory>( "." );
        auto fontURL = dir->get_file( FONTLETTERS_NAME ".xml" );
        auto images = std::make_shared<img_cache>( dir );
        auto texloader = std::make_shared<solosnake_test::quickloader>( rr, images );
        auto fnt = std::make_shared<font>( fontURL, *texloader );

        return std::make_shared<fontprinter>( dimension2d<unsigned int>( WINDOW_W, WINDOW_H ), rr, fnt );
    }

    void init( const rect& screenRect )
    {
        std::vector<std::string> directories;
        directories.push_back( BLUEDATADIR "/meshes" );
        directories.push_back( BLUEDATADIR "/textures" );

        auto assetsDir = std::make_shared<datadirectories>( directories );

        auto url = assetsDir->get_file( "thruster.msh" );
        auto meshes = load_mesh_datas( url, assetsDir );

        g_msrc = meshes.at( 0 );

        g_rr = std::make_shared<deferred_renderer>(
                   screenRect.width(), screenRect.height(), deferred_renderer::HighQuality ); // 1920 x 1080

        g_h = g_rr->load_mesh( g_msrc );

        g_printer = make_test_fontprinter( g_rr );
    }

    // Display 1 stationary cube and 2 nearby rotating cubes.
    void displayFn()
    {
        float cameraDistance = 10.0f;
        float cameraY = 2.50f;

        // Model view - "camera".
        load_translation_4x4( 0.0f, -cameraY, -cameraDistance, g_mv );

        g_rr->set_proj_matrix( g_proj, g_zNear );
        g_rr->set_view_matrix( g_mv );

        g_rr->start_scene();

        const dimension2d<unsigned int> windowSize( g_window_width, g_window_height );

        g_printer->set_text_colour( green );

        g_printer->print_text( utf8text( g_quote ),
                               position2df( 0.5f, 0.0f ),
                               dimension2df( 0.7f, 0.0f ),
                               HorizontalAlignCentre,
                               VerticalAlignTop,
                               1.0f );

        // Finished:
        g_rr->end_scene();

        if( ++g_loopcounter == MAX_TEST_LOOPS )
        {
            g_stop = true;
        }
    }

    void reshapeFn( int w, int h )
    {
        g_window_height = static_cast<unsigned int>( h );
        g_window_width = static_cast<unsigned int>( w );
        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ), float( w ) / h, g_zNear, 100.0f, g_proj );
        g_printer->set_screen_size(
            dimension2d<unsigned int>( static_cast<unsigned int>( w ), static_cast<unsigned int>( h ) ) );
        g_rr->set_viewport( 0, 0, w, h );
    }

    void exitFn( int )
    {
        g_printer.reset();
        g_rr.reset();
    }
}

TEST( fontprinter, text_shows_always )
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
