#define RUN_TESTS

#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include <Windows.h>
#include <gl/GL.h>
#include <cstdint>
#include "solosnake/ioswindow.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/colour.hpp"

using namespace solosnake;

#ifdef WIN32
#pragma comment(lib, "opengl32.lib") // link with Microsoft OpenGL lib
#pragma comment(lib, "glu32.lib")    // link with Microsoft OpenGL Utility lib
#endif

#define COLOUR_SHIFT(c, n) (((std::uint32_t)(std::uint8_t)(c)) << (n))
#define MAKE_COLOUR_RGA(r, g, b)                                                                   \
    ((std::uint32_t)(COLOUR_SHIFT(r, 24) | COLOUR_SHIFT(g, 16) | COLOUR_SHIFT(b, 8)))
#define MAKE_COLOUR_RGBA(r, g, b, a)                                                               \
    ((std::uint32_t)(COLOUR_SHIFT(r, 24) | COLOUR_SHIFT(g, 16) | COLOUR_SHIFT(b, 8)                \
                     | COLOUR_SHIFT(a, 0)))

class iwidgetrenderer
{
public:

    virtual ~iwidgetrenderer()
    {
    }

    virtual void render_quad( int x, int y, size_t width, size_t height, const colour& c ) = 0;

    virtual rect get_window_rect() const = 0;
};

//-------------------------------------------------------------------------

class opengl_widgetrenderer : public iwidgetrenderer
{
public:

    explicit opengl_widgetrenderer( std::shared_ptr<solosnake::ioswindow> w ) : w_( w )
    {
    }

    virtual rect get_window_rect() const
    {
        return w_->get_window_rect();
    }

    // Draws a quad in screen pixel coords with (0,0) as top left.
    virtual void render_quad( int x, int y, size_t width, size_t height, const colour& c )
    {
        // Note: (using OpenGL-centric screen coordinates, with (0,0) in the lower left).
        // OpenGL renders counter clockwise.

        rect r = get_window_rect();

        solosnake::matrix4x4_t ortho2d;

        load_ortho( 0,
                    static_cast<float>( r.width() ),
                    0,
                    static_cast<float>( r.height() ),
                    -1.0,
                    1.0,
                    ortho2d );

        int w = static_cast<int>( width );
        int h = static_cast<int>( height );

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        glMultMatrixf( ortho2d );

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
        glTranslatef( 0.375, 0.375, 0. );

        glDisable( GL_DEPTH_TEST );

        glBegin( GL_QUADS );
        glColor4fv( c.rgba() );
        glVertex2i( x, r.height() - y - h );
        glVertex2i( x + w, r.height() - y - h );
        glVertex2i( x + w, r.height() - y );
        glVertex2i( x, r.height() - y );
        glEnd();
    }

private:
    std::shared_ptr<solosnake::ioswindow> w_;
};

class testwindow : public solosnake::iwindow
{
public:

    explicit testwindow() : alternate_( false ), oswindow_()
    {
    }

    virtual ~testwindow()
    {
    }

    void start( std::shared_ptr<ioswindow> w )
    {
        oswindow_ = w;
        attach_to( w );
        w->show();
    }

    virtual void on_fullscreen_change( const FullscreenState )
    {
    }

    virtual void on_minimised()
    {
    }

    virtual void on_unminimised()
    {
    }

    virtual bool update( long )
    {
        bool continue_looping = false;

        alternate_ = !alternate_;

        if( oswindow() )
        {
            if( alternate_ )
            {
                ::glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
            }
            else
            {
                //::glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
            }

            ::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

            opengl_widgetrenderer wr( oswindow_ );

            wr.render_quad( 1, 1, 20, 10, colour( 255, 0, 255 ) );

            continue_looping = oswindow()->check_messages();
            oswindow()->swap_buffers();
        }

        return continue_looping;
    }

    virtual void on_resized( const solosnake::dimension2d<unsigned int>& newSize )
    {
        glViewport( 0, 0, static_cast<GLsizei>( newSize.width() ), static_cast<GLsizei>( newSize.height() ) );

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        // Calculate The Aspect Ratio Of The Window
        solosnake::matrix4x4_t m;
        solosnake::load_identity_4x4( m );
        // load_perspective( solosnake::radians(PI_RADIANS * 0.25f),
        // float(width)/height, 0.1f, 100.0f, m );

        GLfloat nRange = 10.0;

        if( newSize.width() <= newSize.height() )
        {
            //    load_ortho (-nRange, nRange, -nRange * height / width, nRange
            // * height / width, -nRange, nRange, m);
        }
        else
        {
            //    load_ortho (-nRange * width / height, nRange * width / height,
            // -nRange, nRange, -nRange, nRange, m);
        }

        glMultMatrixf( m );

        glMatrixMode( GL_MODELVIEW ); // Select The Modelview Matrix

        glLoadIdentity();
    }

    virtual void on_dragdropped( const std::vector<std::wstring>& )
    {
    }

private:

    bool                       alternate_;
    std::shared_ptr<ioswindow> oswindow_;
};

typedef std::shared_ptr<testwindow> testwindow_ptr;

TEST( opengl, create_windowed_640x480_opengl_default )
{
    {
        auto w = ioswindow::make_shared( 640, 480, false, "create_windowed_640x480", false, "opengl_test_window", "" );

        testwindow_ptr tw( new testwindow() );

        tw->start( w );

        bool continue_test = true;

        for( size_t i = 0; continue_test && i < 2000; ++i )
        {
            continue_test = tw->update( 0 );
        }

        EXPECT_FALSE( w->is_fullscreen() );
    }
}

TEST( opengl, create_windowed_640x480_opengl21 )
{
    {
         auto w = ioswindow::make_shared( 640, 480, true, "create_windowed_640x480", false, "opengl21_test_window", "opengl 2.1" );

        testwindow_ptr tw( new testwindow() );

        tw->start( w );

        bool continue_test = true;

        for( size_t i = 0; continue_test && i < 2000; ++i )
        {
            continue_test = tw->update( 0 );
        }

        EXPECT_FALSE( w->is_fullscreen() );
    }
}

// TEST(opengl, create_windowed_640x480_opengl31)
//{
//   {
//      std::shared_ptr<ioswindow> w = ioswindow::create( 640, 480,
// OSTXT("create_windowed_640x480"), false, "opengl 3.1" );
//
//      testwindow_ptr tw( new testwindow() );
//
//      tw->start( w );
//
//      bool continue_test = true;
//
//      for( size_t i = 0; continue_test && i<2000;++i)
//      {
//         continue_test = tw->update(0);
//      }
//
//      EXPECT_FALSE( w->is_fullscreen() );
//   }
//}
//
//
// TEST(opengl, create_windowed_640x480_opengl32)
//{
//   {
//      std::shared_ptr<ioswindow> w = ioswindow::create( 640, 480,
// OSTXT("create_windowed_640x480"), false, "opengl 3.3" );
//
//      testwindow_ptr tw( new testwindow() );
//
//      tw->start( w );
//
//      bool continue_test = true;
//
//      for( size_t i = 0; continue_test && i<2000;++i)
//      {
//         continue_test = tw->update(0);
//      }
//
//      EXPECT_FALSE( w->is_fullscreen() );
//   }
//}

// TEST(windows_opengl_window, create_fs_640x480)
//{
//    {
//        std::shared_ptr<ioswindow> w = ioswindow::create( 640, 480,
// OSTXT("create_fs_640x480"), true, "" );
//
//        testwindow_ptr tw( new testwindow() );
//
//        tw->start( w );
//
//        EXPECT_TRUE( w->is_fullscreen() );
//
//        for( size_t i = 0; i<2000; ++i)
//        {
//            tw->update(0);
//        }
//
//        w->set_as_fullscreen( false );
//        w->check_messages();
//    }
//}

#endif
