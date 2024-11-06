//#define RUNTESTS
#ifdef RUNTESTS

#include <memory>
#include "solosnake/testing/testing.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/mesh_cache.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/styledwidgetrenderer.hpp"
#include "solosnake/utf8text.hpp"

// BLUEDATADIR is defined by CMake.

using namespace solosnake;

#define WINDOW_W 1024
#define WINDOW_H 768

// Lets say 100fps = 20s
#define MAX_TEST_LOOPS (100 * 20)

namespace
{
    std::shared_ptr<rendering_system> g_rr;
    std::shared_ptr<styledwidgetrenderer> g_swr;
    bool g_stop = false;
    size_t g_loopcounter = 0;
    unsigned int g_window_width = WINDOW_W;
    unsigned int g_window_height = WINDOW_H;
    matrix4x4_t g_mv;
    matrix4x4_t g_proj;

    void init( std::shared_ptr<window> w )
    {
        auto stylesDir = std::make_shared<datadirectory>( BLUEDATADIR "/styles" );
        auto fontsDir = std::make_shared<datadirectory>( BLUEDATADIR "/fonts" );
        auto modelsDir = std::make_shared<datadirectory>( BLUEDATADIR "/models" );
        auto meshesDir = std::make_shared<datadirectory>( BLUEDATADIR "/meshes" );
        auto texesDir = std::make_shared<datadirectory>( BLUEDATADIR "/textures" );

        auto mshcache = std::make_shared<mesh_cache>( meshesDir, texesDir );
        auto imgcache = std::make_shared<img_cache>( texesDir );

        auto styleFile = stylesDir->get_file( "bluestyle.xml" );

        auto rnd = std::make_shared<rendering_system>( w, deferred_renderer::HighQuality, modelsDir, mshcache, imgcache );

        auto wr = std::make_shared<styledwidgetrenderer>( styleFile, *fontsDir, rnd );

        g_rr.swap( rnd );
        g_swr.swap( wr );
    }

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
        load_translation( 0.0f, -cameraY, -cameraDistance, g_mv );

        g_rr->renderer().set_proj_matrix( g_proj );
        g_rr->renderer().set_view_matrix( g_mv );
        g_rr->renderer().set_ambiant_light( 0.1f, 0.1f, 0.1f );

        g_rr->renderer().start_scene();

        quad q( 0.1f, 0.1f, 0.2f, 0.1f );
        widgetstate buttonstate = 0;

        if( g_swr )
        {
            g_swr->render_button_back( q, buttonstate );
            g_swr->render_text( utf8text( "Hello World" ), 1, q, buttonstate );
            g_swr->render_button_highlight( q, buttonstate );
        }

        // Finished:
        g_rr->renderer().end_scene();

        if( ++g_loopcounter == MAX_TEST_LOOPS )
        {
            g_stop = true;
        }
    }

    void reshapeFn( int w, int h )
    {
        g_window_width = static_cast<unsigned int>( w );

        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ), float( w ) / h, 0.1f, 100.0f, g_proj );
    }

    void exitFn( int )
    {
        g_swr.reset();
        g_rr.reset();
    }
}

TEST( styledwidgetrenderer, ctor )
{
    try
    {
        auto glut = glutWindow::create( 3, 3, WINDOW_W, WINDOW_H );

        EXPECT_TRUE( GL_NO_ERROR == glGetError() );

        glut->stopDisplayBool( &g_stop );
        glut->glutExitFunc( exitFn );
        glut->glutDisplayFunc( displayFn );
        glut->glutReshapeFunc( reshapeFn );

        init( glut );

        glut->glutMainLoop();

        /*       rr.reset();*/
        g_swr.reset();
    }
    catch( const std::exception& e )
    {
        ADD_FAILURE(e.what());
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }

    g_swr.reset();
    g_rr.reset();
}

#endif