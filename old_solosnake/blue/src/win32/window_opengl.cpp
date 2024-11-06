#define NOMINMAX
#include <algorithm>
#include <cassert>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>
#include "solosnake/logging.hpp"
#include "solosnake/noexcept.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/opengl_debug_output.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/utf8_to_unicode.hpp"
#include "solosnake/win32/includewindows.hpp"
#include "solosnake/win32/window_opengl.hpp"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#pragma comment(lib, "opengl32.lib") // link with Microsoft OpenGL lib
#pragma comment(lib, "glu32.lib")    // link with Microsoft OpenGL Utility lib

#ifndef GET_X_LPARAM
#   define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#   define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// These flags can also be found in wglew.h
#ifndef __wglew_h__
#   define WGL_TYPE_RGBA_ARB                        (0x202B)
#   define WGL_SUPPORT_OPENGL_ARB                   (0x2010)
#   define WGL_DRAW_TO_WINDOW_ARB                   (0x2001)
#   define WGL_ACCELERATION_ARB                     (0x2003)
#   define WGL_FULL_ACCELERATION_ARB                (0x2027)
#   define WGL_RED_BITS_ARB                         (0x2015)
#   define WGL_GREEN_BITS_ARB                       (0x2017)
#   define WGL_BLUE_BITS_ARB                        (0x2019)
#   define WGL_ALPHA_BITS_ARB                       (0x201B)
#   define WGL_DEPTH_BITS_ARB                       (0x2022)
#   define WGL_STENCIL_BITS_ARB                     (0x2023)
#   define WGL_DOUBLE_BUFFER_ARB                    (0x2011)
#   define WGL_PIXEL_TYPE_ARB                       (0x2013)
#   define WGL_SAMPLE_BUFFERS_ARB                   (0x2041)
#   define WGL_SAMPLES_ARB                          (0x2042)
#   define WGL_CONTEXT_MAJOR_VERSION_ARB            (0x2091)
#   define WGL_CONTEXT_MINOR_VERSION_ARB            (0x2092)
#   define WGL_CONTEXT_FLAGS_ARB                    (0x2094)
#   define WGL_CONTEXT_DEBUG_BIT_ARB                (0x0001)
#   define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB   (0x0002)
#endif // __wglew_h__

// Define to 0 to disable writing extensions to clog.
#define SS_LOG_EXTENSIONS                           (0)

// Windows does not support fewer than 8 bpp anymore. Use this value
// wherever colour bpp is requested:
#define SS_WINDOW_COLOUR_BITS_PER_PIXEL             (8u)

extern "C"
{
    // We define our own, GLEW independent function pointers here, so that we can
    // init GLEW with the final OpenGL context.
    typedef BOOL( APIENTRY* FNWGLCHOOSEPIXELFORMATARB )( HDC, const int*, const FLOAT*, UINT, int*, UINT* );
    typedef HGLRC( APIENTRY* FNWGLCREATECONTEXTATTRIBSARB )( HDC, HGLRC, const int* );

    struct driverfunctions
    {
        FNWGLCHOOSEPIXELFORMATARB    choosePixelFormatFn;
        FNWGLCREATECONTEXTATTRIBSARB createContextAttribsFn;
    };
}

namespace solosnake
{
    namespace
    {
        // Declarations.

        bool check_OpenGL();

        HDC get_dc( const HWND );

        dimension2dui get_screen_size();

        std::string get_windows_error_as_string( const DWORD );

        driverfunctions init_OpenGL_create_functions( const HWND );

        void show_taskbar( const bool visible );

        void alter_window( const LONG x, const LONG y, const LONG w, const LONG h,
                           const HWND, const DWORD style, const DWORD exstyle );

        // Implementations.

        driverfunctions init_OpenGL_create_functions( const HWND hTmp )
        {
            driverfunctions funcs = { nullptr, nullptr };

            const HDC hDC = get_dc( hTmp );

            // Set a dummy pixel format so that we can get access to wgl functions
            const PIXELFORMATDESCRIPTOR pfd = PIXELFORMATDESCRIPTOR();
            ::SetPixelFormat( hDC, 1, &pfd );

            // Create OGL context and make it current
            const HGLRC glRc = ::wglCreateContext( hDC );

            if( NULL == glRc )
            {
                ss_err( "wglCreateContext(HDC) failed : ",
                        get_windows_error_as_string( ::GetLastError() ) );
                ::ReleaseDC( hTmp, hDC );
                ss_throw( "wglCreateContext failed." );
            }

            if( FALSE == ::wglMakeCurrent( hDC, glRc ) )
            {
                ss_err( "wglMakeCurrent failed : ", get_windows_error_as_string( ::GetLastError() ) );
                ::wglDeleteContext( glRc );
                ::ReleaseDC( hTmp, hDC );
                ss_throw( "wglMakeCurrent failed." );
            }

            const char* const vendor     = reinterpret_cast<const char*>( glGetString( GL_VENDOR ) );
            const char* const version    = reinterpret_cast<const char*>( glGetString( GL_VERSION ) );
            const char* const renderer   = reinterpret_cast<const char*>( glGetString( GL_VENDOR ) );
            const char* const language   = reinterpret_cast<const char*>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
            const char* const extensions = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );

            if( vendor )
            {
                ss_log( vendor );
            }

            if( version )
            {
                ss_log( version );
            }

            if( renderer )
            {
                ss_log( renderer );
            }

            if( language )
            {
                ss_log( language );
            }

            if( extensions )
            {
                if( solosnake::g_ssLogLvl > SS_LOG_WRN )
                {
#if SS_LOG_EXTENSIONS
                    using namespace std;
                    istringstream iss( extensions );
                    copy( istream_iterator<string>( iss ),
                          istream_iterator<string>(),
                          ostream_iterator<string>( *g_clog, "\n" ) );
#endif
                }
            }

            funcs.choosePixelFormatFn = reinterpret_cast<FNWGLCHOOSEPIXELFORMATARB>(
                                            wglGetProcAddress( "wglChoosePixelFormatARB" ) );

            if( funcs.choosePixelFormatFn == nullptr )
            {
                ::wglMakeCurrent( NULL, NULL );
                ::wglDeleteContext( glRc );
                ::ReleaseDC( hTmp, hDC );
                ss_err( "wglChoosePixelFormatARB not found." );
                ss_throw( "Failed to find required OpenGL extension 'wglChoosePixelFormatARB'" );
            }

            check_OpenGL();

            funcs.createContextAttribsFn =
                reinterpret_cast<FNWGLCREATECONTEXTATTRIBSARB>( wglGetProcAddress( "wglCreateContextAttribsARB" ) );

            if( funcs.createContextAttribsFn == nullptr )
            {
                ::wglMakeCurrent( NULL, NULL );
                ::wglDeleteContext( glRc );
                ::ReleaseDC( hTmp, hDC );
                ss_throw( "Failed to find required OpenGL extension 'wglCreateContextAttribsARB'" );
            }
            else
            {
                ss_dbg( "pwglCreateContextAttribsARB is 0x",
                        static_cast<const void*>( funcs.createContextAttribsFn ) );
            }

            check_OpenGL();

            // Release resources - we have the minimum required function pointers to
            // setup a newer OpenGL context.
            ::wglMakeCurrent( NULL, NULL );
            ::wglDeleteContext( glRc );
            ::ReleaseDC( hTmp, hDC );

            return funcs;
        }

        bool check_OpenGL()
        {
            switch( glGetError() )
            {
                case GL_NO_ERROR:
                    return true;

                case GL_INVALID_ENUM:
                    ss_err( "GL_INVALID_ENUM" );
                    break;

                case GL_INVALID_VALUE:
                    ss_err( "GL_INVALID_VALUE" );
                    break;

                case GL_INVALID_OPERATION:
                    ss_err( "GL_INVALID_OPERATION" );
                    break;

                case GL_STACK_OVERFLOW:
                    ss_err( "GL_STACK_OVERFLOW" );
                    break;

                case GL_STACK_UNDERFLOW:
                    ss_err( "GL_STACK_UNDERFLOW" );
                    break;

                case GL_OUT_OF_MEMORY:
                    ss_err( "GL_OUT_OF_MEMORY" );
                    break;

                default:
                    ss_err( "Unknown OpenGL error code." );
                    break;
            }

            assert( glGetError() == GL_NO_ERROR );

            return false;
        }

        dimension2dui get_screen_size()
        {
            return dimension2dui( static_cast<unsigned int>( ::GetSystemMetrics( SM_CXSCREEN ) ),
                                  static_cast<unsigned int>( ::GetSystemMetrics( SM_CYSCREEN ) ) );
        }

        std::string get_windows_error_as_string( const DWORD dwError )
        {
            std::string error;

            LPVOID lpMsgBuf = 0;

            if( 0u != ::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                                        | FORMAT_MESSAGE_IGNORE_INSERTS,
                                        NULL,
                                        dwError,
                                        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                                        reinterpret_cast<LPSTR>( & lpMsgBuf ),
                                        0u,
                                        NULL ) )
            {
                error = std::string( static_cast<LPSTR>( lpMsgBuf ) );
                ::LocalFree( lpMsgBuf );
            }

            return error;
        }

        bool windowclass_is_already_registered( const HINSTANCE hInstance, const TCHAR* windowclassname )
        {
            WNDCLASS details;
            const BOOL foundClass = ::GetClassInfo( hInstance, windowclassname, &details );
            return FALSE != foundClass;
        }

        HDC get_dc( const HWND h )
        {
            const HDC dc = ::GetDC( h );

            if( NULL == dc )
            {
                ss_err( "GetDC(HWND) failed: ", get_windows_error_as_string( ::GetLastError() ) );
                ss_throw( "GetDC(HWND) failed." );
            }

            return dc;
        }

        HWND create_window( const DWORD style,
                            const DWORD exstyle,
                            const TCHAR* const windowclassname,
                            const int w,
                            const int h,
                            const HINSTANCE hInstance,
                            const TCHAR* const title,
                            windowswindowopengl* const p = NULL )
        {
            ss_dbg( "create_window" );

            const int x = ( ::GetSystemMetrics( SM_CXSCREEN ) - w ) / 2;
            const int y = ( ::GetSystemMetrics( SM_CYSCREEN ) - h ) / 2;

            // Create The Main Window
            const HWND hWnd = ::CreateWindowExW( exstyle,
                                                 windowclassname,
                                                 title,     // Window title.
                                                 style,
                                                 x,         // Window Position.
                                                 y,         // Window Position.
                                                 w,         // Window width.
                                                 h,         // Window height.
                                                 NULL,      // Parent Window.
                                                 NULL,      // No Menu.
                                                 hInstance, // Instance.
                                                 p );       // Pass pointer to platform to WM_CREATE

            if( 0 == hWnd )
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ).c_str() );
                ss_throw( "CreateWindowExW failed." );
            }

            return hWnd;
        }

        //////////////////////////////////////////////////////////////////////////

        class opengldc
        {
        public:

            opengldc( const HWND hWnd,
                      const driverfunctions& funcs,
                      const unsigned int bits,
                      const unsigned int zbits,
                      const unsigned int fsaa,
                      const std::pair<int, int>& openGlVersion,
                      const bool allowDeprecatedOpenGL,
                      const bool debuggingDC );

            ~opengldc();

            void swap_buffers()
            {
                assert( hDC_ );
                ::SwapBuffers( hDC_ );
            }

        private:
            opengldc( const opengldc& );
            opengldc& operator=( const opengldc& );

        private:
            HWND    hWnd_;
            HDC     hDC_;
            HGLRC   hGLRC_;
        };

        //////////////////////////////////////////////////////////////////////////

        opengldc::opengldc( const HWND hWnd,
                            const driverfunctions& funcs,
                            const unsigned int bits,
                            const unsigned int zbits,
                            const unsigned int fsaa,
                            const std::pair<int, int>& openGlVersion,
                            const bool allowDeprecatedOpenGL,
                            const bool debuggingDC )
            : hWnd_( hWnd )
            , hDC_( 0 )
            , hGLRC_( 0 )
        {
            ss_dbg( "opengldc ctor" );

            const HDC hDC = get_dc( hWnd_ );

            // Specify the important attributes we care about
            int pixAttribs[] =
            {
                WGL_SUPPORT_OPENGL_ARB,
                GL_TRUE,                   // 0, Must support OGL rendering
                WGL_DRAW_TO_WINDOW_ARB,
                GL_TRUE,                   // 2, pf that can run a window
                WGL_ACCELERATION_ARB,
                WGL_FULL_ACCELERATION_ARB, // 4, must be HW accelerated
                WGL_RED_BITS_ARB,
                static_cast<int>( bits ),  // 6,
                WGL_GREEN_BITS_ARB,
                static_cast<int>( bits ),  // 8,
                WGL_BLUE_BITS_ARB,
                static_cast<int>( bits ),  // 10,
                WGL_ALPHA_BITS_ARB,
                static_cast<int>( bits ),  // 12,
                WGL_DEPTH_BITS_ARB,
                static_cast<int>( zbits ), // 14, bits of depth precision for window
                WGL_STENCIL_BITS_ARB,
                1,                         // 16, Stencil buffer
                WGL_DOUBLE_BUFFER_ARB,
                GL_TRUE,                   // 18, Double buffered context
                WGL_PIXEL_TYPE_ARB,
                WGL_TYPE_RGBA_ARB,         // 20, pf should be RGBA type
                WGL_SAMPLE_BUFFERS_ARB,
                GL_TRUE,                   // 22, MSAA on
                WGL_SAMPLES_ARB,
                static_cast<int>( fsaa ),  // 24, MSAA amount
                0 // NUL terminator.
            };

            if( 0 == fsaa )
            {
                pixAttribs[23] = GL_FALSE;
                pixAttribs[24] = 0;
            }

            // Ask OpenGL to find the most relevant format matching our attribs
            // Only get one format back.
            int pixelFormat = -1;
            UINT pixCount = 0;

            assert( funcs.choosePixelFormatFn );
            assert( funcs.createContextAttribsFn );

            if( FALSE == funcs.choosePixelFormatFn( hDC, pixAttribs, NULL, 1, &pixelFormat, &pixCount ) )
            {
                ss_err( "wglChoosePixelFormatARB failed : ",
                        get_windows_error_as_string( ::GetLastError() ) );
                ::ReleaseDC( hWnd_, hDC );
                ss_throw( "wglChoosePixelFormatARB failed." );
            }

            if( -1 == pixelFormat ) // Did Windows Find A Matching Pixel Format?
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ) );
                ::ReleaseDC( hWnd_, hDC );
                ss_throw( "wglChoosePixelFormatARB failed." );
            }

            PIXELFORMATDESCRIPTOR pfd = PIXELFORMATDESCRIPTOR();

            // Are We Able To Set The Pixel Format?
            if( TRUE != ::SetPixelFormat( hDC, pixelFormat, &pfd ) )
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ) );
                ::ReleaseDC( hWnd, hDC );
                ss_throw( "SetPixelFormat failed." );
            }

            GLint attribs[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, openGlVersion.first,  // 0,1
                WGL_CONTEXT_MINOR_VERSION_ARB, openGlVersion.second, // 2,3
                WGL_CONTEXT_FLAGS_ARB,         0,                    // 4,5
                0
            };

            if( openGlVersion.first >= 3 && ( false == allowDeprecatedOpenGL ) )
            {
                // No deprecated features allowed.
                attribs[5] |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
            }

            if( debuggingDC )
            {
                // Slower but logging internally enabled in OpenGL.
                attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
            }

            const HGLRC hGLRC = funcs.createContextAttribsFn( hDC, 0, attribs );

            // Are we able to get a rendering context?
            if( !hGLRC )
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ) );
                ::ReleaseDC( hWnd, hDC );
                ss_throw( "wglCreateContext failed." );
            }

            // Try to activate the rendering context.
            if( TRUE != ::wglMakeCurrent( hDC, hGLRC ) )
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ) );
                ::wglDeleteContext( hGLRC );
                ::ReleaseDC( hWnd, hDC );
                ss_throw( "wglMakeCurrent failed." );
            }

            check_OpenGL();

            if( false == enable_opengl_debugging( debuggingDC ) )
            {
                if( debuggingDC )
                {
                    ss_wrn( "Unable to enable OpenGL debug output." );
                }
            }

            hGLRC_ = hGLRC;
            hDC_   = hDC;
        }

        opengldc::~opengldc()
        {
            try
            {
                ss_dbg( "opengldc dtor" );

                if( hGLRC_ )
                {
                    if( GL_NO_ERROR != glGetError() )
                    {
                        ss_wrn( "glGetError() was non zero at shutdown." );
                    }

                    if( TRUE != ::wglMakeCurrent( NULL, NULL ) )
                    {
                        ss_err( "(windowswindowopengl) wglMakeCurrent failed." );
                    }

                    if( TRUE != ::wglDeleteContext( hGLRC_ ) )
                    {
                        ss_err( "(windowswindowopengl) wglDeleteContext failed." );
                    }

                    hGLRC_ = NULL;
                }

                if( NULL != hDC_ )
                {
                    // "The return value indicates whether the DC was released."
                    // "If the DC was released, the return value is 1." - MSDN.
                    if( 1 != ::ReleaseDC( hWnd_, hDC_ ) )
                    {
                        ss_err( "(windowswindowopengl) Release Device Context Failed." );
                        hDC_ = NULL;
                    }
                }

                hWnd_ = NULL;

                ss_log( "(windowswindowopengl)  opengldc destroyed." );
            }
            catch( ... )
            {
            }
        }

        //////////////////////////////////////////////////////////////////////////

        void show_taskbar( const bool visible )
        {
            // Base DEVMODE on current DEVMODE.
            DEVMODE dmScreenSettings =  DEVMODE();
            dmScreenSettings.dmSize = sizeof( dmScreenSettings );
            dmScreenSettings.dmDriverExtra = 0;

            if( ::EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings ) )
            {
                // To get rid of the task bar use CDS_FULLSCREEN.
                const DWORD flags  = visible ? 0u : CDS_FULLSCREEN;
                const auto changed = ::ChangeDisplaySettingsEx( NULL, &dmScreenSettings, NULL, flags, NULL );
                if( DISP_CHANGE_SUCCESSFUL  != changed )
                {
                    ss_wrn( "Unable to set CDS_FULLSCREEN." );
                }
            }
        }

        void alter_window( const LONG x,
                           const LONG y,
                           const LONG w,
                           const LONG h,
                           const HWND hwnd,
                           const DWORD style,
                           const DWORD exstyle )
        {
            ::SetWindowLong( hwnd, GWL_STYLE,   static_cast<LONG>( style ) );
            ::SetWindowLong( hwnd, GWL_EXSTYLE, static_cast<LONG>( exstyle ) );

            RECT cr;
            ::SetRect( &cr, 0, 0, w, h );
            ::AdjustWindowRectEx( &cr, style, FALSE, exstyle );
            ::SetWindowPos( hwnd, HWND_TOPMOST, x, y, cr.right - cr.left, cr.bottom - cr.top, SWP_SHOWWINDOW );
            ::SetForegroundWindow( hwnd );
            ::InvalidateRect( hwnd, NULL, TRUE );
        }
    }

    //////////////////////////////////////////////////////////////////////////

    class windowswindowopengl::windowhandle
    {
    public:

        windowhandle( const std::wstring& title,
                      const DWORD dwExStyle,
                      const DWORD dwStyle,
                      const std::wstring& wclassname,
                      const int width,
                      const int height,
                      const HINSTANCE hInstance,
                      const BYTE bits,
                      const BYTE zbits,
                      const unsigned int fsaa,
                      const std::pair<int, int>& openGlVersion,
                      const bool allowDeprecatedOpenGL,
                      const bool debuggingOpenGL,
                      windowswindowopengl* const p );

        ~windowhandle() SS_NOEXCEPT;

        operator HWND () const
        {
            return hWnd_;
        }

        const void* as_void_ptr() const
        {
            return static_cast<const void*>( &hWnd_ );
        }

        void swap_buffers()
        {
            assert( dc_ );
            assert( check_OpenGL() );
            if( dc_ )
            {
                dc_->swap_buffers();
            }
        }

    private:

        windowhandle( const windowhandle& ) = delete;
        windowhandle& operator=( const windowhandle& ) = delete;

    private:
        HWND                        hWnd_;
        std::unique_ptr<opengldc>   dc_;
    };

    //////////////////////////////////////////////////////////////////////////

    windowswindowopengl::windowhandle::windowhandle( const std::wstring& title,
                                                     const DWORD dwExStyle,
                                                     const DWORD dwStyle,
                                                     const std::wstring& wclassname,
                                                     const int width,
                                                     const int height,
                                                     const HINSTANCE hInstance,
                                                     const BYTE bits,
                                                     const BYTE zbits,
                                                     const unsigned int fsaa,
                                                     const std::pair<int, int>& openGlVersion,
                                                     const bool allowDeprecatedOpenGL,
                                                     const bool debuggingOpenGL,
                                                     windowswindowopengl* const p )
        : hWnd_( 0 )
        , dc_( nullptr )
    {
        ss_dbg( "windowhandle" );

        assert( !wclassname.empty() );

        // Create the window to init GLEW with.
        const HWND hTmp = create_window( dwStyle,
                                         dwExStyle,
                                         &wclassname[0],
                                         width,
                                         height,
                                         hInstance,
                                         title.c_str(),
                                         NULL );

        const driverfunctions funcs = init_OpenGL_create_functions( hTmp );

        ::DestroyWindow( hTmp );

        // Create The Main Window
        hWnd_ = create_window( dwStyle,
                               dwExStyle,
                               &wclassname[0],
                               width,
                               height,
                               hInstance,
                               title.c_str(),
                               p );

        // Potentially throws here if the OpenGL version is unsupported.
        dc_.reset( new opengldc( hWnd_,
                                 funcs,
                                 bits,
                                 zbits,
                                 fsaa,
                                 openGlVersion,
                                 allowDeprecatedOpenGL,
                                 debuggingOpenGL ) );

        check_OpenGL();
    }

    windowswindowopengl::windowhandle::~windowhandle() SS_NOEXCEPT
    {
        try
        {
            ss_dbg( "~windowhandle" );

            dc_.reset();

            if( hWnd_ )
            {
                ::DestroyWindow( hWnd_ );
                hWnd_ = 0;
            }

            ss_log( "(windowswindowopengl)  windowhandle destroyed." );
        }
        catch( ... )
        {
        }
    }

    //////////////////////////////////////////////////////////////////////////

    LRESULT CALLBACK windowswindowopengl::wndproc( HWND hWnd, UINT msg, WPARAM wPrm, LPARAM lPrm )
    {
        LRESULT result = 0;
        LONG_PTR p = ::GetWindowLongPtr( hWnd, 0 );

        if( msg == WM_CREATE )
        {
            assert( 0 == p );

            // Get the windowswindowopengl pointer from the
            // CREATESTRUCT and store it in the window and embed
            // it into the window struct to use as the callback window
            // message handler.
            const CREATESTRUCT* const cs = reinterpret_cast<const CREATESTRUCT*>( lPrm );
            ::SetWindowLongPtr( hWnd, 0, reinterpret_cast<LONG_PTR>( cs->lpCreateParams ) );
            result = DefWindowProc( hWnd, msg, wPrm, lPrm );
        }
        else if( msg == WM_DESTROY )
        {
            if( p && hWnd )
            {
                // Window being destroyed: remove the special embedded handler callback.
                ::SetWindowLongPtr( hWnd, 0, 0 );
                p = LONG_PTR();
            }

            result = DefWindowProc( hWnd, msg, wPrm, lPrm );
        }
        else if( p )
        {
            auto pwingl = reinterpret_cast<windowswindowopengl*>( p );
            result = pwingl->msg_proc( hWnd, msg, wPrm, lPrm );
        }
        else
        {
            result = DefWindowProc( hWnd, msg, wPrm, lPrm );
        }

        return result;
    }

    windowswindowopengl::windowswindowopengl( const windowswindowopengl::params& init )
        : active_wnd_classname_( utf8_to_unicode( init.classname ) )
        , tmp_wnd_classname_( std::wstring(L"TMP") + active_wnd_classname_ )
        , hInstance_( ::GetModuleHandle( NULL ) )
        , hCursor_( HCURSOR() )
        , winhandle_()
        , showparam_( SW_SHOWNORMAL )
        , screenSize_( get_screen_size() )
        , alt_ctrl_shift_( 0u )
        , windowedX_()
        , windowedY_()
        , windowedStyle_()
        , windowedExStyle_()
        , clipCursorWhenFullscreen_( true )
        , showCursorWhenFullscreen_( init.fullscreencursor )
        , windowed_( true )
        , allowResizing_( init.allowResizing )
        , minimized_( false )
        , maximized_( false )
        , continue_update_( true )
        , dragDropOn_( false )
    {
        if( active_wnd_classname_.empty() )
        {
            ss_throw( "Window active classname cannot be empty." );
        }

        // When clipCursorWhenFullscreen_ is true, the cursor is limited to
        // the device window when the app goes full-screen.  This prevents users
        // from accidentally clicking outside the app window on a multi monitor
        // system. This flag is turned off by default for debug builds, since it
        // makes multi monitor debugging difficult.
#ifdef NDEBUG
        clipCursorWhenFullscreen_ = false;
#else
        clipCursorWhenFullscreen_ = true;
#endif

        open_window( init );
    }

    windowswindowopengl::~windowswindowopengl() SS_NOEXCEPT
    {
        try
        {
            // Causes WM Destroy / Quit sequence on Windows.
            post_close_msg();

            winhandle_.reset();

            if( ::UnregisterClass( &tmp_wnd_classname_[0], hInstance_ ) )
            {
                ss_log( "(windowswindowopengl)  tmp window class unregistered." );
            }
            else
            {
                ss_log( "(windowswindowopengl)  Unable to unregister tmp window class." );
            }

            if( ::UnregisterClass( &active_wnd_classname_[0], hInstance_ ) )
            {
                ss_log( "(windowswindowopengl)  active window class unregistered." );
            }
            else
            {
                ss_log( "(windowswindowopengl)  Unable to unregister active class." );
            }

            // Reset any graphics settings changes we might have made.
            ::ChangeDisplaySettings( NULL, 0 );
        }
        catch( ... )
        {
        }
    }

    //! windowclassname is the name of the class of the window registered
    //! with the operating system. This can be searched for in an OS specific
    //! manner to see if an instance of this application is running.
    void windowswindowopengl::open_window( const windowswindowopengl::params& init )
    {
        if( winhandle_ )
        {
            ss_throw( "Window previously created." );
        }

        if( hInstance_ == NULL )
        {
            ss_throw( "Invalid hInstance found." );
        }

        if( !( init.zDepth == 32 || init.zDepth == 24 || init.zDepth == 16 ) )
        {
            ss_throw( "Invalid z-bit depth specified. Only 32|24|16 allowed." );
        }

        register_window_class( active_wnd_classname_, static_cast<WNDPROC>( &windowswindowopengl::wndproc ) );
        register_window_class( tmp_wnd_classname_,    nullptr );
        create_and_show_window( init );
        set_as_fullscreen( init.fullscreen );

        ss_log( "os::windows::windowswindowopengl created." );
    }

    void windowswindowopengl::register_window_class( const std::wstring& classname, const WNDPROC wndProc )
    {
        assert( !classname.empty() );

        if( !windowclass_is_already_registered( hInstance_, classname.c_str() ) )
        {
            WNDCLASS wndClass;
            std::memset( &wndClass, 0, sizeof( wndClass ) );

            // Redraw On Size, And Own DC For Window. receives dbl clicks.
            wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;

            // WndProc Handles Messages
            wndClass.lpfnWndProc = wndProc;

            // No Extra Window Data
            wndClass.cbClsExtra = 0;

            wndClass.hIcon   = ::LoadIcon( hInstance_, TEXT( "ID_SOLOSNAKE_WINDOW_ICON" ) );
            wndClass.hCursor = ::LoadCursor( NULL, IDC_ARROW );

            hCursor_ = wndClass.hCursor;

            wndClass.hInstance     = hInstance_;    // Set The Instance
            wndClass.hbrBackground = NULL;          // No Background Required For GL
            wndClass.lpszMenuName  = NULL;          // We Don't Want A Menu
            wndClass.lpszClassName = &classname[0]; // Set The Class Name

            wndClass.cbWndExtra = sizeof( windowswindowopengl* );

            ATOM window_class = ::RegisterClass( &wndClass );

            if( 0 == window_class )
            {
                ss_err( get_windows_error_as_string( ::GetLastError() ) );
                ss_throw( "RegisterClass failed." );
            }
        }
    }

    void windowswindowopengl::create_and_show_window( const windowswindowopengl::params& init )
    {
        assert( winhandle_ == 0 );

        windowedX_ = 0;
        windowedY_ = 0;
        windowedStyle_   = dwstyle( windowed_ );
        windowedExStyle_ = dwexstyle( windowed_ );

        // Try to find a window that matches the user's requested size, but
        // shrink if it would be larger than the screen.
        const unsigned int sw = screenSize_.width();
        const unsigned int sh = screenSize_.height();
        windowedWidth_  = init.width  > sw ? static_cast<LONG>( sw ) : static_cast<LONG>( init.width );
        windowedHeight_ = init.height > sh ? static_cast<LONG>( sh ) : static_cast<LONG>( init.height );
        RECT windowRect = { 0, 0, 0, 0 };
        windowRect.left   = ( static_cast<LONG>( sw ) - windowedWidth_ )  / 2;
        windowRect.top    = ( static_cast<LONG>( sh ) - windowedHeight_ ) / 2;
        windowRect.right  = windowRect.left + windowedWidth_;
        windowRect.bottom = windowRect.top  + windowedHeight_;

        // Expand window rect so that it enclosed the desired client rect.
        if( FALSE == ::AdjustWindowRectEx( &windowRect, windowedStyle_, FALSE, windowedExStyle_ ) )
        {
            ss_err( get_windows_error_as_string( ::GetLastError() ) );
            ss_throw( "AdjustWindowRectEx failed." );
        }

        // Create The Window.
        // This may very likely throw if, for example, the requested OpenGL version
        // is not supported on the current platform/driver/hardware.
        winhandle_ = std::make_unique<windowhandle>( utf8_to_unicode( init.title ),
                                                     windowedExStyle_,
                                                     windowedStyle_,
                                                     active_wnd_classname_,
                                                     windowRect.right - windowRect.left,
                                                     windowRect.bottom - windowRect.top,
                                                     hInstance_,
                                                     static_cast<BYTE>( SS_WINDOW_COLOUR_BITS_PER_PIXEL ),
                                                     static_cast<BYTE>( init.zDepth ),
                                                     init.fsaa,
                                                     init.openGlVersion,
                                                     init.allowDeprecatedOpenGL,
                                                     init.enableDebugging,
                                                     this );

        if( !windowed_ && !init.fullscreencursor )
        {
            ::ShowCursor( FALSE ); // Hide Mouse Pointer
        }
        else
        {
            ::ShowCursor( TRUE );
        }
    }

    void windowswindowopengl::show()
    {
        assert( continue_update_ );

        if( is_shutdown() )
        {
            ss_throw( "Attempting to use (show) window which has received WM_CLOSE." );
        }

        const int showparam = showparam_;
        showparam_ = SW_SHOW;

        ::ShowWindow( *winhandle_, showparam );
        ::SetForegroundWindow( *winhandle_ );
        ::SetFocus( *winhandle_ );

        handle_possible_size_change();
    }

    void windowswindowopengl::swap_buffers()
    {
        assert( winhandle_ );
        winhandle_->swap_buffers();
    }

    void windowswindowopengl::post_close_msg()
    {
        if( continue_update_ && winhandle_ )
        {
            ::PostMessage( *winhandle_, WM_CLOSE, 0, 0 );
            check_messages();
            assert( !continue_update_ );
        }

        ss_log( "(windowswindowopengl)  Exiting client loop." );
    }

    // params = oldwindow and newwindow.
    void windowswindowopengl::on_attached( const std::shared_ptr<iwindow>&,
                                           const std::shared_ptr<iwindow>& )
    {
        // NOP
    }

    //! Will continue to return true until WM_CLOSE is received.
    bool windowswindowopengl::check_messages()
    {
        _ASSERTE( _CrtCheckMemory() );

        MSG msg;
        bool gotMsg = ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        while( gotMsg )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );

            gotMsg = ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );
        }

        return continue_update_;
    }

    bool windowswindowopengl::is_fullscreen() const
    {
        return !windowed_;
    }

    bool windowswindowopengl::is_shutdown() const
    {
        return !continue_update_;
    }

    void windowswindowopengl::minimise()
    {
        ::ShowWindow( *winhandle_, SW_MINIMIZE );
        minimised();
    }

    void windowswindowopengl::minimised()
    {
        ss_log( "(windowswindowopengl) Minimising." );

        if( attached_window() )
        {
            attached_window()->on_minimised();
        }
    }

    void windowswindowopengl::unminimise()
    {
        ::ShowWindow( *winhandle_, SW_RESTORE );
        unminimised();
    }

    void windowswindowopengl::unminimised()
    {
        ss_log( "(windowswindowopengl) Un-Minimising." );
        if( attached_window() )
        {
            attached_window()->on_unminimised();
        }
    }

    DWORD windowswindowopengl::dwstyle( const bool windowed ) const
    {
        return windowed ? windowed_dwstyle() : fullscreen_dwstyle();
    }

    DWORD windowswindowopengl::dwexstyle( const bool windowed ) const
    {
        return windowed ? windowed_dwexstyle() : fullscreen_dwexstyle();
    }

    DWORD windowswindowopengl::windowed_dwstyle() const
    {
        DWORD dwStyle( WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );

        if( allowResizing_ )
        {
            dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
        }

        return dwStyle;
    }

    DWORD windowswindowopengl::fullscreen_dwstyle() const
    {
        return DWORD( WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );
    }

    DWORD windowswindowopengl::windowed_dwexstyle() const
    {
        return DWORD( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE );
    }

    DWORD windowswindowopengl::fullscreen_dwexstyle() const
    {
        return DWORD( WS_EX_APPWINDOW );
    }

    void windowswindowopengl::handle_possible_size_change()
    {
        ss_log( "(windowswindowopengl)  handle_possible_size_change() called." );

        RECT clientRect = { 0, 0, 0, 0 };
        ::GetClientRect( *winhandle_, &clientRect );

        const unsigned int newW = static_cast<unsigned int>( clientRect.right  - clientRect.left );
        const unsigned int newH = static_cast<unsigned int>( clientRect.bottom - clientRect.top );

        // A new window size will require a new back-buffer
        // size, so the 3D structures must be changed accordingly.
        if( attached_window() )
        {
            attached_window()->on_resized( Dimension2d<unsigned int>( newW, newH ) );
        }
    }

    rect windowswindowopengl::get_window_rect() const
    {
        RECT clientRect = { 0, 0, 0, 0 };
        ::GetClientRect( *winhandle_, &clientRect );
        assert( clientRect.right  >= clientRect.left );
        assert( clientRect.bottom >= clientRect.top );

        return rect( 0, 0,
                     static_cast<unsigned int>( clientRect.right  - clientRect.left ),
                     static_cast<unsigned int>( clientRect.bottom - clientRect.top ) );
    }

    rect windowswindowopengl::get_screen_rect() const
    {
        const dimension2dui wh( static_cast<unsigned int>( ::GetSystemMetrics( SM_CXSCREEN ) ),
                                static_cast<unsigned int>( ::GetSystemMetrics( SM_CYSCREEN ) ) );

        return rect( 0, 0, wh );
    }

    void windowswindowopengl::set_window_title( const std::string& utf8title )
    {
        ::SetWindowText( *winhandle_, utf8_to_unicode( utf8title ).data() );
    }

    RECT windowswindowopengl::fullscreen_rect() const
    {
        RECT windowRect;
        ::SetRect( &windowRect, 0, 0,
                   static_cast<int>( screenSize_.width() ),
                   static_cast<int>( screenSize_.height() ) );

        return windowRect;
    }

    void windowswindowopengl::set_as_fullscreen( const bool wantFS )
    {
        bool changed = false;

        // Show wait cursor.
        const HCURSOR cursor = ::SetCursor( ::LoadCursor( NULL, IDC_WAIT ) );

        if( windowed_ )
        {
            if( wantFS )
            {
                show_taskbar( false );

                // Windowed, fullscreen wanted.
                // Store the client rect and the styles. We will restore these upon
                // exiting fullscreen. We cannot calculate these as they may have been
                // changed by minimizing and maximizing the window frame.
                RECT cr;
                ::GetClientRect( *winhandle_, &cr );
                RECT wr;
                ::GetWindowRect( *winhandle_, &wr );
                windowedX_ = wr.left;
                windowedY_ = wr.top;
                windowedWidth_   = cr.right - cr.left;
                windowedHeight_  = cr.bottom - cr.top;
                windowedStyle_   = static_cast<DWORD>( ::GetWindowLong( *winhandle_, GWL_STYLE ) );
                windowedExStyle_ = static_cast<DWORD>( ::GetWindowLong( *winhandle_, GWL_EXSTYLE ) );

                alter_window( 0, 0,
                              static_cast<LONG>( screenSize_.width() ),
                              static_cast<LONG>( screenSize_.height() ),
                              *winhandle_, fullscreen_dwstyle(), fullscreen_dwexstyle() );

                changed   = true;
                windowed_ = false;
            }
        }
        else
        {
            if( ! wantFS )
            {
                // Full-screen, windowed wanted
                alter_window( windowedX_, windowedY_, windowedWidth_, windowedHeight_,
                              *winhandle_, windowedStyle_, windowedExStyle_ );
                windowed_ = true;
                changed   = true;
                show_taskbar( true );
            }
        }

        ::SetCursor( cursor );

        handle_possible_size_change();

        if( changed )
        {
            if( attached_window() )
            {
                attached_window()->on_fullscreen_change( windowed_ ? NotFullscreened : Fullscreened );
            }
        }
    }

    bool windowswindowopengl::drag_and_drop_enabled() const
    {
        return dragDropOn_;
    }

    bool windowswindowopengl::enable_drag_and_drop( const bool enable )
    {
        assert( winhandle_ );
        ::DragAcceptFiles( *winhandle_, enable ? TRUE : FALSE );
        dragDropOn_ = enable;
        return dragDropOn_;
    }

    const void* windowswindowopengl::get_window_handle() const
    {
        return winhandle_->as_void_ptr();
    }

    void windowswindowopengl::handle_drag_drop( const HDROP drop ) SS_NOEXCEPT
    {
        const HCURSOR cursor = ::SetCursor( ::LoadCursor( NULL, IDC_WAIT ) );

        try
        {
            TCHAR filename[MAX_PATH] = { 0 };
            const UINT filecount = ::DragQueryFileW( drop, 0xFFFFFFFF, NULL, 0 );

            std::vector<std::wstring> files;
            files.reserve( filecount );

            for( UINT i = 0; i < filecount; ++i )
            {
                if( 0u != ::DragQueryFileW( drop, i, filename, MAX_PATH ) )
                {
                    files.push_back( std::wstring( filename ) );
                }
            }

            if( attached_window() )
            {
                attached_window()->on_dragdropped( files );
            }
        }
        catch( const std::exception& e )
        {
            ss_err( "(windowswindowopengl::handle_drag_drop) exception handled and ignored: ", e.what() );
        }

        ::DragFinish( drop );
        ::SetCursor( cursor );
    }

    LRESULT windowswindowopengl::msg_proc( HWND hWnd, UINT msg, WPARAM wPrm, LPARAM lPrm )
    {
        LRESULT result = LRESULT();

        assert( ::GetWindowLong( hWnd, 0 ) );

        // In all handled cases, break-ing will return the result, which unless
        // changed is Zero. The switch 'default' case will perform the default
        // Windows MSG handling.
        switch( msg )
        {
            case( WM_KILLFOCUS ) :
            {
                alt_ctrl_shift_ = 0u;
                hCursor_ = ::GetCursor();
                input_changes().add_event( input_event::make_focus_lost() );
            }
            break;

            case( WM_SETFOCUS ) :
            {
                alt_ctrl_shift_ = 0u;
                ::SetCursor( hCursor_ );
                input_changes().add_event( input_event::make_focus_gained() );
            }
            break;

            case( WM_CHAR ) :
            {
                // Text received.
                assert( wPrm <= std::numeric_limits<wchar_t>::max() );
                input_changes().add_event( input_event::make_text_entry( static_cast<wchar_t>( wPrm ) ) );
            }
            break;

            case( WM_KEYDOWN ) :
            {
                assert( wPrm <= 255u );

                if( VK_SHIFT == wPrm || VK_LSHIFT == wPrm || VK_RSHIFT == wPrm )
                {
                    alt_ctrl_shift_ |= input_event::shift_down;
                }
                else if( VK_CONTROL == wPrm || VK_LCONTROL == wPrm || VK_RCONTROL == wPrm )
                {
                    alt_ctrl_shift_ |= input_event::ctrl_down;
                }
                else if( VK_MENU == wPrm )
                {
                    alt_ctrl_shift_ |= input_event::alt_down;
                }

                // "An application should return zero if it processes this message. "
                input_changes().add_event( input_event::make_key_down( static_cast<uint16_t>( wPrm ), alt_ctrl_shift_ ) );

#ifndef NDEBUG
                if( VK_PAUSE == wPrm )
                {
                    ::PostMessage( hWnd, WM_CLOSE, 0, 0 );
                }
                else if( VK_F11 == wPrm )
                {
                    set_as_fullscreen( windowed_ );
                }
#endif
            }
            break;

            case( WM_KEYUP ) :
            {
                assert( wPrm <= 255u );

                if( VK_SHIFT == wPrm || VK_LSHIFT == wPrm || VK_RSHIFT == wPrm )
                {
                    alt_ctrl_shift_ &= ~input_event::shift_down;
                }
                else if( VK_CONTROL == wPrm || VK_LCONTROL == wPrm || VK_RCONTROL == wPrm )
                {
                    alt_ctrl_shift_ &= ~input_event::ctrl_down;
                }
                else if( VK_MENU == wPrm )
                {
                    alt_ctrl_shift_ &= ~input_event::alt_down;
                }

                input_changes().add_event( input_event::make_key_up( static_cast<uint16_t>( wPrm ), alt_ctrl_shift_ ) );
            }
            break;

            case WM_GETMINMAXINFO:
            {
                if( allowResizing_ )
                {
                    // MINMAXINFO* p = reinterpret_cast<MINMAXINFO*>(lPrm);
                    result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                }
                else
                {
                    // If resizing is not allowed, override this message.
                    // MINMAXINFO* p = reinterpret_cast<MINMAXINFO*>(lPrm);
                }
            }
            break;

            // Double clicks:

            // Note: Windows will take care of the switch between primary
            // and secondary (left and right) mouse buttons. Thus this
            // msg will always be sent by the primary mouse button, whether
            // left or right handed.

            // lPrm
            // The low-order word specifies the x-coordinate of the cursor. The
            // coordinate is relative to the upper-left corner of the client area.
            // The high-order word specifies the y-coordinate of the cursor. The
            // coordinate is relative to the upper-left corner of the client area.

            case WM_LBUTTONDBLCLK:
            {
                input_changes().add_event(
                    input_event::make_cursor_dbl_click(
                        CURSOR_PRIMARY_BUTTON_FLAG | ( wPrm & ( MK_SHIFT | MK_CONTROL ) ),
                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_MBUTTONDBLCLK:
            {
                input_changes().add_event(
                    input_event::make_cursor_dbl_click(
                        CURSOR_MIDDLE_BUTTON_FLAG | ( wPrm & ( MK_SHIFT | MK_CONTROL ) ),
                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_RBUTTONDBLCLK:
            {
                input_changes().add_event(
                    input_event::make_cursor_dbl_click(
                        CURSOR_SECONDARY_BUTTON_FLAG | ( wPrm & ( MK_SHIFT | MK_CONTROL ) ),
                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            // Horizontal position of cursor
            // xPos = LOWORD(lParam);

            // Specifies the x-coordinate of the cursor. The coordinate
            // is relative to the upper-left corner of the client area.

            // Vertical position of cursor
            // yPos = HIWORD(lParam);

            // Specifies the y-coordinate of the cursor. The coordinate
            // is relative to the upper-left corner of the client area.

            // Mouse down:

            case WM_LBUTTONDOWN:
            {
                ::SetCapture( hWnd );
                input_changes().add_event(
                    input_event::make_cursor_button_down( CURSOR_PRIMARY_BUTTON_FLAG,
                                                          screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_MBUTTONDOWN:
            {
                ::SetCapture( hWnd );
                input_changes().add_event(
                    input_event::make_cursor_button_down( CURSOR_MIDDLE_BUTTON_FLAG,
                                                          screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_RBUTTONDOWN:
            {
                ::SetCapture( hWnd );
                input_changes().add_event(
                    input_event::make_cursor_button_down( CURSOR_SECONDARY_BUTTON_FLAG,
                                                          screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            // Mouse up:

            case WM_LBUTTONUP:
            {
                ::ReleaseCapture();
                input_changes().add_event(
                    input_event::make_cursor_button_up( CURSOR_PRIMARY_BUTTON_FLAG,
                                                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_MBUTTONUP:
            {
                ::ReleaseCapture();
                input_changes().add_event(
                    input_event::make_cursor_button_up( CURSOR_MIDDLE_BUTTON_FLAG,
                                                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            case WM_RBUTTONUP:
            {
                ::ReleaseCapture();
                input_changes().add_event(
                    input_event::make_cursor_button_up( CURSOR_SECONDARY_BUTTON_FLAG,
                                                        screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            // Mouse move:

            case WM_MOUSEMOVE:
            {
                input_changes().add_event(
                    input_event::make_cursor_move( static_cast<uint16_t>( wPrm ),
                                                   screenxy( static_cast<uint32_t>( lPrm ) ) ) );
            }
            break;

            // The high-order word indicates the distance the wheel is rotated,
            // expressed in multiples or divisions of WHEEL_DELTA, which is 120.
            // A positive value indicates that the wheel was rotated forward, away
            // from the user; a negative value indicates that the wheel was rotated
            // backward, toward the user.
            case WM_MOUSEWHEEL:
            {
                POINT p;
                p.x = GET_X_LPARAM( lPrm );
                p.y = GET_Y_LPARAM( lPrm );

                if( ::ScreenToClient( hWnd, &p ) )
                {
                    // p now contains transformed points.

                    assert( p.x >= std::numeric_limits<std::int16_t>::min() );
                    assert( p.x <= std::numeric_limits<std::int16_t>::max() );
                    assert( p.y >= std::numeric_limits<std::int16_t>::min() );
                    assert( p.y <= std::numeric_limits<std::int16_t>::max() );

                    RECT rect;
                    ::GetClientRect( hWnd, &rect );

                    if( p.x < ( rect.right  - rect.left ) &&
                            p.y < ( rect.bottom - rect.top ) )
                    {
                        input_changes().add_event(
                            input_event::make_wheelmove( GET_WHEEL_DELTA_WPARAM( wPrm ),
                                                         screenxy( static_cast<std::uint16_t>( p.x ),
                                                                   static_cast<std::uint16_t>( p.y ) ) ) );
                    }
                }
            }
            break;

            case WM_SIZE:
            {
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );

                // The low-order word of lParam specifies the new width of the client area.
                // The high-order word of lParam specifies the new height of the client area.
                const WORD newW = HIWORD( lPrm );
                const WORD newH = LOWORD( lPrm );
                ss_log( "WM_SIZE ", newW, " ", newH );

                if( SIZE_MAXHIDE == wPrm )
                {
                    // Message is sent to all pop-up windows when some other window
                    // is maximized.
                    ss_log( "(windowswindowopengl)  WM_SIZE (SIZE_MAXHIDE) message received." );
                }
                else if( SIZE_MAXSHOW == wPrm )
                {
                    // Message is sent to all pop-up windows when some other window
                    // has been restored to its former size.
                    ss_log( "(windowswindowopengl)  WM_SIZE (SIZE_MAXSHOW) message received." );
                }
                else if( SIZE_MINIMIZED == wPrm )
                {
                    ss_log( "(windowswindowopengl)  WM_SIZE (SIZE_MINIMIZED) message received." );

                    if( clipCursorWhenFullscreen_ && !windowed_ )
                    {
                        ::ClipCursor( NULL );
                    }

                    minimized_ = true;
                    maximized_ = false;
                }
                else if( SIZE_MAXIMIZED == wPrm )
                {
                    // Message is sent to all pop-up windows when some other window
                    // is maximized.

                    ss_log( "(windowswindowopengl)  WM_SIZE (SIZE_MAXIMIZED) message received." );

                    handle_possible_size_change();

                    if( minimized_ )
                    {
                        // Un-pause since we're no longer minimized
                    }

                    minimized_ = false;
                    maximized_ = true;
                }
                else if( SIZE_RESTORED == wPrm )
                {
                    // The window has been resized, but neither the SIZE_MINIMIZED nor
                    // SIZE_MAXIMIZED value applies.

                    ss_log( "(windowswindowopengl)  WM_SIZE (SIZE_RESTORED) message received." );

                    if( maximized_ )
                    {
                        maximized_ = false;
                        handle_possible_size_change();
                    }
                    else if( minimized_ )
                    {
                        minimized_ = false;
                        handle_possible_size_change();
                    }
                    else
                    {
                        handle_possible_size_change();
                    }
                }
            }
            break; // WM_SIZE

            case WM_EXITSIZEMOVE:
            {
                ss_log( "(windowswindowopengl)  WM_EXITSIZEMOVE message received." );
                handle_possible_size_change();
            }
            break;

            // Active state of window is changing...
            case WM_ACTIVATE:
            case WM_ACTIVATEAPP:
            {
                // Going inactive...
                if( 0 == wPrm ) // WM_INACTIVE ?
                {
                    if( !windowed_ )
                    {
                        // Fullscreen going inactive - probably ALT TAB...
                        ss_log( "(windowswindowopengl)  WM_ACTIVATE with WM_INACTIVE "
                                "message received - possible ALT TAB." );

                        if( !is_minimised() )
                        {
                            minimise();
                        }
                    }
                }
                else
                {
                    if( !windowed_ )
                    {
                        if( is_minimised() )
                        {
                            // Re-activated window.
                            ss_log( "(windowswindowopengl)  WM_ACTIVATE un-pausing window." );

                            unminimise();
                        }
                    }
                }
            }
            break;

            case WM_SETCURSOR:
            {
                // Turn off Windows cursor in fullscreen mode
                if( !is_minimised() && !windowed_ && !showCursorWhenFullscreen_ )
                {
                    ::SetCursor( NULL );

                    // Prevent Windows from setting cursor to window class cursor
                    result = TRUE;
                }
                else
                {
                    result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                }
            }
            break;

            case WM_DROPFILES:
            {
                handle_drag_drop( reinterpret_cast<HDROP>( wPrm ) );
            }
            break;

            case WM_NCHITTEST:
            {
                if( windowed_ )
                {
                    result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                }
                else
                {
                    // Prevent the user from selecting the menu in fullscreen mode
                    result = HTCLIENT;
                }
            }
            break;

            case WM_POWERBROADCAST:
            {
                switch( wPrm )
                {
#ifndef PBT_APMQUERYSUSPEND
#   define PBT_APMQUERYSUSPEND 0x0000
#endif
                    case PBT_APMQUERYSUSPEND:
                        // At this point, the app should save any data for open
                        // network connections, files, etc., and prepare to go into
                        // a suspended mode.
                        result = TRUE;
                        break;

#ifndef PBT_APMRESUMESUSPEND
#   define PBT_APMRESUMESUSPEND 0x0007
#endif
                    case PBT_APMRESUMESUSPEND:
                        // At this point, the app should recover any data, network
                        // connections, files, etc., and resume running from when
                        // the app was suspended.
                        result = TRUE;
                        break;

                    default:
                        result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                        break;
                }
            }
            break;

            case WM_SYSCOMMAND:
            {
                if( windowed_ )
                {
                    // We want to ignore the ALT key as it seems interrupts the process: the ALT key
                    // begins activation of the window title menu, normally allowing the user to move
                    // the window etc. However in our case it seems to actually interrupt the app.
                    if( wPrm == SC_KEYMENU && (lPrm >> 16) <= 0)
                    {
                        result = FALSE;
                    }
                    else
                    {
                        result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                    }
                }
                else
                {
                    // Prevent moving/sizing and power loss in fullscreen mode
                    switch( wPrm )
                    {
                        case SC_MOVE:
                        case SC_SIZE:
                        case SC_MAXIMIZE:
                        case SC_KEYMENU:
                        case SC_MONITORPOWER:
                            result = TRUE;
                            break;

                        default:
                            result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                            break;
                    }
                }
            }
            break;

            case WM_ENTERMENULOOP:
            {
                // Menu is starting to be displayed.
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
            }
            break;

            case WM_EXITMENULOOP:
            {
                // Menu is stopping being displayed.
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
            }
            break;

            case WM_COMMAND:
            {
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
            }
            break;

            case WM_CLOSE:
            {
                ss_log( "(windowswindowopengl)  WM_CLOSE" );
                continue_update_ = false;
                input_changes().shutdown();
            }
            break;

            case( WM_DESTROY ):
            {
                ss_log( "(windowswindowopengl)  WM_DESTROY" );
                ::PostQuitMessage(0);
                // Window is closing down and DestroyWindow has/is being called on it.
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
            }
            break;

            default:
                result = ::DefWindowProc( hWnd, msg, wPrm, lPrm );
                break;
        }

        return result;
    }
}
