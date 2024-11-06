#include "solosnake/x11/x11.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include <iomanip>

namespace solosnake
{
    X11Display::X11Display() : display_( XOpenDisplay( 0 ) )
    {
        if( nullptr == display_ )
        {
            ss_throw( "Unable to open X Display." );
        }

        int glx_major, glx_minor;

        if( !glXQueryVersion( display_, &glx_major, &glx_minor ) )
        {
            ss_throw( "Unable to query glx version." );
        }

        if( ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
        {
            ss_throw( "glx version less than 1.3." );
        }
        else
        {
            ss_log( "GLX ", glx_major, ".", glx_minor );
        }
    }

    X11Display::~X11Display()
    {
        if( display_ )
        {
            XCloseDisplay( display_ );
            display_ = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    X11ColorMap::X11ColorMap( XVisualInfo* vi, Display* d )
        : owner_( d ), cmap_( XCreateColormap( d, RootWindow( d, vi->screen ), vi->visual, AllocNone ) )
    {
    }

    X11ColorMap::~X11ColorMap()
    {
        XFreeColormap( owner_, cmap_ );
    }

    //////////////////////////////////////////////////////////////////////////

    X11FBConfig::X11FBConfig( GLXFBConfig* c ) : config_( c )
    {
    }

    X11FBConfig::~X11FBConfig()
    {
        if( config_ )
        {
            XFree( config_ );
            config_ = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    X11VisualInfo::X11VisualInfo( XVisualInfo* vi ) : vi_( vi )
    {
    }

    X11VisualInfo::~X11VisualInfo()
    {
        if( vi_ )
        {
            XFree( vi_ );
            vi_ = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    X11Window::X11Window( const char* name, unsigned int width, unsigned int height, unsigned int zDepth )
        : display_( new X11Display() )
        , window_( 0L )
        , delete_window_atom_()
        , bestFBConfig_( 0 )
    {
        // See
        // http://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html
        // http://tronche.com/gui/x/xlib/events/processing-overview.html

        ss_log( "Z-Depth = ", zDepth );

        int visual_attribs[]
            = { GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,           GLX_CONFIG_CAVEAT, GLX_NONE,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,           GLX_RENDER_TYPE,   GLX_RGBA_BIT,
                GLX_RED_SIZE,      8,                        GLX_GREEN_SIZE,    8,
                GLX_BLUE_SIZE,     8,                        GLX_ALPHA_SIZE,    8,
                GLX_DEPTH_SIZE,    static_cast<int>( zDepth ), GLX_STENCIL_SIZE,  8,
                GLX_DOUBLEBUFFER,  True,
                //             //GLX_SAMPLE_BUFFERS  , 1,
                //             //GLX_SAMPLES         , 4,
                None
              };

        Display* d = *display_;
        int fbcount;
        GLXFBConfig* p = glXChooseFBConfig( d, DefaultScreen( d ), visual_attribs, &fbcount );

        if( p == nullptr || fbcount == 0 )
        {
            if( nullptr != glXChooseFBConfig( d, DefaultScreen( d ), NULL, &fbcount ) )
            {
                ss_wrn( "Is direct rendering available? Check using 'glxinfo | grep "
                        "rendering'" );
            }

            ss_throw( "No Frame Buffer config found to match required attributes." );
        }

        std::unique_ptr<X11FBConfig> fbc( new X11FBConfig( p ) );

        int best_fbc = -1;
        int worst_fbc = -1;
        int best_num_samp = -1;
        int worst_num_samp = 999;

        GLXFBConfig* fbcArray = *fbc;

        for( int i = 0; i < fbcount; ++i )
        {
            X11VisualInfo vi( glXGetVisualFromFBConfig( d, fbcArray[i] ) );

            if( vi )
            {
                int samp_buf, samples;
                glXGetFBConfigAttrib( d, fbcArray[i], GLX_SAMPLE_BUFFERS, &samp_buf );
                glXGetFBConfigAttrib( d, fbcArray[i], GLX_SAMPLES, &samples );

                if( ( best_fbc < 0 ) || ( samp_buf && samples > best_num_samp ) )
                {
                    best_fbc = i;
                    best_num_samp = samples;
                }

                if( ( worst_fbc < 0 ) || ( !samp_buf || samples < worst_num_samp ) )
                {
                    worst_fbc = i;
                    worst_num_samp = samples;
                }
            }
        }

        // Record best fb config for use creating OpenGL context:
        bestFBConfig_ = fbcArray[best_fbc];
        fbc.reset();

        X11VisualInfo xvi( glXGetVisualFromFBConfig( d, bestFBConfig_ ) );
        XVisualInfo* vi = xvi;

        cmap_.reset( new X11ColorMap( vi, d ) );
        XSetWindowAttributes swa;
        swa.colormap = *cmap_;
        swa.background_pixmap = None;
        swa.border_pixel = 0;
        swa.bit_gravity = StaticGravity;
        swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
                         | PointerMotionMask | VisibilityChangeMask | EnterWindowMask | LeaveWindowMask
                         | StructureNotifyMask | ExposureMask | FocusChangeMask;

        window_ = XCreateWindow( d,
                                 RootWindow( d, vi->screen ),
                                 0,
                                 0,
                                 width,
                                 height,
                                 0,
                                 vi->depth,
                                 InputOutput,
                                 vi->visual,
                                 CWBorderPixel | CWColormap | CWEventMask | CWBitGravity,
                                 &swa );

        if( 0L == window_ )
        {
            ss_throw( "Unable to create X11 window." );
        }

        ss_log( "X Window = ", window_ );

        XStoreName( d, window_, name );
        XMapWindow( d, window_ ); 
        
        delete_window_atom_ = XInternAtom( d, "WM_DELETE_WINDOW", False ); 
        XSetWMProtocols( d, window_, &delete_window_atom_, 1 );  

        ss_log( "X11Window created" );
    }

    X11Window::~X11Window()
    {
        // This is the order we want shutdown in: XDestroyWindow, XFreeColormap,
        // XCloseDisplay.
        if( display_ )
        {
            Display* d = *display_;
            XDestroyWindow( d, window_ );
            cmap_.reset();
            display_.reset();
            ss_log( "X11Window destroyed" );
        }
    }
}