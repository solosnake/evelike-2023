#include "solosnake/x11/window_opengl.hpp"
#include "solosnake/x11/x11.hpp"
#include "solosnake/input_event.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/opengl_debug_output.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/vkeys.hpp"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <X11/keysymdef.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

// Code originally based on:
// http://www.opengl.org/wiki/Tutorial:_OpenGL_3.0_Context_Creation_%28GLX%29

#define CONTEXT_MAJOR_VERSION_ARB           (0x2091u)
#define CONTEXT_MINOR_VERSION_ARB           (0x2092u)
#define TYPE_RGBA_ARB                       (0x202Bu)
#define SUPPORT_OPENGL_ARB                  (0x2010u)
#define DRAW_TO_WINDOW_ARB                  (0x2001u)
#define ACCELERATION_ARB                    (0x2003u)
#define FULL_ACCELERATION_ARB               (0x2027u)
#define RED_BITS_ARB                        (0x2015u)
#define GREEN_BITS_ARB                      (0x2017u)
#define BLUE_BITS_ARB                       (0x2019u)
#define ALPHA_BITS_ARB                      (0x201Bu)
#define DEPTH_BITS_ARB                      (0x2022u)
#define STENCIL_BITS_ARB                    (0x2023u)
#define DOUBLE_BUFFER_ARB                   (0x2011u)
#define PIXEL_TYPE_ARB                      (0x2013u)
#define SAMPLE_BUFFERS_ARB                  (0x2041u)
#define SAMPLES_ARB                         (0x2042u)
#define CONTEXT_MAJOR_VERSION_ARB           (0x2091u)
#define CONTEXT_MINOR_VERSION_ARB           (0x2092u)
#define CONTEXT_FLAGS_ARB                   (0x2094u)
#define CONTEXT_DEBUG_BIT_ARB               (0x0001u)
#define CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  (0x0002u)

// The CTRL key is down.
#define WIN32_MK_CONTROL                    (0x0008u)
// The left mouse button is down.
#define WIN32_MK_LBUTTON                    (0x0001u)
// The middle mouse button is down.
#define WIN32_MK_MBUTTON                    (0x0010u)
// The right mouse button is down.
#define WIN32_MK_RBUTTON                    (0x0002u)
// The SHIFT key is down.
#define WIN32_MK_SHIFT                      (0x0004u)
// The first X button is down.
#define WIN32_MK_XBUTTON1                   (0x0020u)
// The second X button is down.
#define WIN32_MK_XBUTTON2                   (0x0040u)

#define NOT_A_MOUSE_BUTTON_ID               (0x1234)
#define DOUBLE_CLICK_MS                     (150)

namespace solosnake
{
    namespace
    {

        /*
         *   if ((keysym >= XK_space) && (keysym <= XK_asciitilde)){
         * printf ("Ascii key:- ");
         * if (event->state & ShiftMask)
         *        printf("(Shift) %s\n", buffer);
         * else if (event->state & LockMask)
         *        printf("(Caps Lock) %s\n", buffer);
         * else if (event->state & ControlMask)
         *        printf("(Control) %c\n", 'a'+ buffer[0]-1) ;
         * else printf("%s\n", buffer) ;
         }
         else if ((keysym >= XK_Shift_L) && (keysym <= XK_Hyper_R)){
           printf ("modifier key:- ");
           switch (keysym){
             case XK_Shift_L: printf("Left Shift\n"); break;
             case XK_Shift_R: printf("Right Shift\n");break;
             case XK_Control_L: printf("Left Control\n");break;
             case XK_Control_R: printf("Right Control\n"); break;
             case XK_Caps_Lock: printf("Caps Lock\n"); break;
             case XK_Shift_Lock: printf("Shift Lock\n");break;
             case XK_Meta_L: printf("Left Meta\n");  break;
             case XK_Meta_R: printf("Right Meta\n"); break;

           }
         }
         else if ((keysym >= XK_Left) && (keysym <= XK_Down)){
           printf("Arrow Key:-");
           switch(keysym){
             case XK_Left: printf("Left\n");break;
             case XK_Up: printf("Up\n");break;
             case XK_Right: printf("Right\n");break;
             case XK_Down: printf("Down\n");break;
           }
         }
         else if ((keysym >= XK_F1) && (keysym <= XK_F35)){
           printf ("function key %d pressed\n", keysym - XK_F1);

           if (buffer == NULL)
             printf("No matching string\n");
           else
             printf("matches <%s>\n",buffer);
         }

         else if ((keysym == XK_BackSpace) || (keysym == XK_Delete)){
           printf("Delete\n");
         }

         else if ((keysym >= XK_KP_0) && (keysym <= XK_KP_9)){
           printf("Number pad key %d\n", keysym -  XK_KP_0);
         }
         else if (keysym == XK_Break) {
           printf("closing display\n");
           XCloseDisplay(display);
           exit (0);
         }else{
           printf("Not handled\n");
         }
         if ((keysym >= XK_space) && (keysym <= XK_asciitilde)){
           printf ("Ascii key:- ");
           if (event->state & ShiftMask)
             printf("(Shift) %s\n", buffer);
           else if (event->state & LockMask)
             printf("(Caps Lock) %s\n", buffer);
           else if (event->state & ControlMask)
             printf("(Control) %c\n", 'a'+ buffer[0]-1) ;
           else printf("%s\n", buffer) ;
        }
        else if ((keysym >= XK_Shift_L) && (keysym <= XK_Hyper_R)){
        printf ("modifier key:- ");
        switch (keysym){
          case XK_Shift_L: printf("Left Shift\n"); break;
          case XK_Shift_R: printf("Right Shift\n");break;
          case XK_Control_L: printf("Left Control\n");break;
          case XK_Control_R: printf("Right Control\n"); break;
          case XK_Caps_Lock: printf("Caps Lock\n"); break;
          case XK_Shift_Lock: printf("Shift Lock\n");break;
          case XK_Meta_L: printf("Left Meta\n");  break;
          case XK_Meta_R: printf("Right Meta\n"); break;

        }
        }
        else if ((keysym >= XK_Left) && (keysym <= XK_Down)){
        printf("Arrow Key:-");
        switch(keysym){
          case XK_Left: printf("Left\n");break;
          case XK_Up: printf("Up\n");break;
          case XK_Right: printf("Right\n");break;
          case XK_Down: printf("Down\n");break;
        }
        }
        else if ((keysym >= XK_F1) && (keysym <= XK_F35)){
        printf ("function key %d pressed\n", keysym - XK_F1);

        if (buffer == NULL)
          printf("No matching string\n");
        else
          printf("matches <%s>\n",buffer);
        }

        else if ((keysym == XK_BackSpace) || (keysym == XK_Delete)){
        printf("Delete\n");
        }

        else if ((keysym >= XK_KP_0) && (keysym <= XK_KP_9)){
        printf("Number pad key %d\n", keysym -  XK_KP_0);
        }
        else if (keysym == XK_Break) {
        printf("closing display\n");
        XCloseDisplay(display);
        exit (0);
        }else{
        printf("Not handled\n");
        }
        */

        static std::uint16_t translate_xkey_to_vkey( KeySym keysym )
        {
            ss_log( "keysym ", static_cast<unsigned int>(keysym) );

            // 0 - 9, A - Z, 36, 36 total
            if( ( keysym >= '0' && keysym <= '9' ) || ( keysym >= 'A' && keysym <= 'Z' ) )
            {
                return keysym;
            }

            // a - z, 26, 72 total
            else if( keysym >= 'a' && keysym <= 'z' )
            {
                // Move 32 down to get 'A' from 'a' in the ASCII set. VK_A == 'A'
                // not 'a'.
                return keysym - 32u;
            }

            // Numpad 0 - 9, 10, 82 total
            else if( keysym >= XK_KP_0 && keysym <= XK_KP_9 )
            {
                static_assert( ( XK_KP_9 - XK_KP_0 ) == 9, "Range XK_KP_0 to XK_KP_9 is not 10" );
                return VK_NUMPAD0 + ( keysym - XK_KP_0 );
            }

            // F1 - F24, 24, 106 total
            else if( keysym >= XK_F1 && keysym <= XK_F24 )
            {
                ss_log( keysym - XK_F1 );
                static_assert( ( XK_F24 - XK_F1 ) == 23, "Range XK_F1 to XK_F24 is not 24" );
                return VK_F1 + ( keysym - XK_F1 );
            }

            else if( ( keysym >= XK_Shift_L ) && ( keysym <= XK_Hyper_R ) )
            {
                switch( keysym )
                {
                    case XK_Shift_L:
                        return VK_SHIFT; // VK_LSHIFT;
                        break;
                    case XK_Shift_R:
                        return VK_SHIFT; // VK_RSHIFT;
                        break;
                    case XK_Control_L:
                        return VK_LCONTROL;
                        break;
                    case XK_Control_R:
                        return VK_RCONTROL;
                        break;
                    case XK_Caps_Lock:
                        return VK_CAPITAL;
                        break;
                    // case XK_Shift_Lock: printf("Shift Lock\n");break;
                    // case XK_Meta_L:     printf("Left Meta\n");  break;
                    // case XK_Meta_R:     printf("Right Meta\n"); break;
                    default:
                        return 0u; // Invalid VK code.
                }
            }

            else if( keysym >= XK_Left && keysym <= XK_Down )
            {
                switch( keysym )
                {
                    case XK_Left:
                        return VK_LEFT;
                        break;
                    case XK_Up:
                        return VK_UP;
                        break;
                    case XK_Right:
                        return VK_RIGHT;
                        break;
                    case XK_Down:
                        return VK_DOWN;
                        break;
                    default:
                        return 0u; // Invalid VK code.
                }
            }

            else
            {
                std::uint16_t vkey = 0u;

                switch( keysym )
                {
                    case XK_BackSpace:
                        vkey = VK_BACK;
                        break;
                    case XK_Tab:
                        vkey = VK_TAB;
                        break;
                    case XK_Clear:
                        vkey = VK_CLEAR;
                        break;
                    case XK_Return:
                        vkey = VK_RETURN;
                        break;
                    case XK_Pause:
                    case XK_Break:
                        vkey = VK_PAUSE;
                        break;
                    case XK_Escape:
                        vkey = VK_ESCAPE;
                        break;
                    case XK_Delete:
                        vkey = VK_DELETE;
                        break;
                    case XK_space:
                        vkey = VK_SPACE;
                        break;
                    default:
                        vkey = 0u; // Invalid VK code.
                }

                return vkey;
            }
        }

        typedef GLXContext( *glXCreateContextAttribsARBProc )( Display*, GLXFBConfig, GLXContext, Bool, const int* );

        //! The state member is set to indicate the logical state of the pointer
        //! buttons and modifier keys just prior to the event, which is the bitwise
        //! inclusive OR of one or more of the button or modifier key masks:
        //! Button1Mask, Button2Mask, Button3Mask, Button4Mask, Button5Mask,
        //! ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask,
        //! Mod4Mask, and Mod5Mask.
        static inline std::uint16_t make_cursor_stateflags( const unsigned int state )
        {
            std::uint16_t win32state = 0u;

            if( state & ControlMask )
            {
                win32state |= WIN32_MK_CONTROL;
            }

            if( state & ShiftMask )
            {
                win32state |= WIN32_MK_SHIFT;
            }

            if( state & Button1Mask )
            {
                win32state |= WIN32_MK_LBUTTON;
            }

            if( state & Button2Mask )
            {
                win32state |= WIN32_MK_MBUTTON;
            }

            if( state & Button3Mask )
            {
                win32state |= WIN32_MK_RBUTTON;
            }

            return win32state;
        }

        static std::unique_ptr<X11Window> make_window( const char* name,
                                                       unsigned int w,
                                                       unsigned int h,
                                                       unsigned int zDepth );

        static bool isExtensionSupported( const char* extList, const char* extension );

        // Helper to check for extension string presence.  Adapted from:
        //   http://www.opengl.org/resources/features/OGLextensions/
        static bool isExtensionSupported( const char* extList, const char* extension )
        {
            assert( GL_NO_ERROR == glGetError() );
            const char* start;
            const char* where, *terminator;

            /* Extension names should not have spaces. */
            where = strchr( extension, ' ' );

            if( where || *extension == '\0' )
            {
                return false;
            }

            // It takes a bit of care to be fool-proof about parsing the
            // OpenGL extensions string. Don't be fooled by sub-strings,
            //  etc.
            for( start = extList;; )
            {
                where = strstr( start, extension );

                if( !where )
                {
                    break;
                }

                terminator = where + strlen( extension );

                if( where == start || *( where - 1 ) == ' ' )
                {
                    if( *terminator == ' ' || *terminator == '\0' )
                    {
                        return true;
                    }
                }

                start = terminator;
            }

            return false;
        }

        bool g_ctxErrorOccurred = false;

        static int ctxErrorHandlerFn( Display* d, XErrorEvent* e )
        {
            char buffer_return[1024] = { 0 };
            XGetErrorText( d, e->error_code, buffer_return, sizeof( buffer_return ) / sizeof( char ) );
            buffer_return[1023] = 0;
            ss_err( "X: ", buffer_return );
            g_ctxErrorOccurred = true;
            return 0;
        }

        static inline std::unique_ptr<X11Window> make_window( const char* name,
                                                              const unsigned int w,
                                                              const unsigned int h,
                                                              const unsigned int zDepth )
        {
            return std::unique_ptr<X11Window>( new X11Window( name, w, h, zDepth ) );
        }
    }

    ////////////////////////////////////////////////////////////////////////////

    x11windowopenglparams::x11windowopenglparams()
        : width( 800 )
        , height( 600 )
        , zDepth( 24 )
        , stencil( 8 )
        , openGlVersion( std::make_pair( 3, 3 ) )
        , allowOldOpenGL( false )
        , enableDebugging( false )
    {
    }

    x11windowopengl::x11windowopengl( const char* title, const x11windowopenglparams& params )
        : window_( make_window( title, params.width, params.height, params.zDepth ) )
        , context_( 0 )
        , windowWidth_( 0 )
        , windowHeight_( 0 )
        , alt_ctrl_shift_( 0u )
        , mouse_button_released_id_( NOT_A_MOUSE_BUTTON_ID )
        , mouse_release_time_( 0u )
        , continue_update_( true )
    {
        Display* const d = window_->display();

        // Create OpenGL context.
        typedef GLXContext( *glXCreateContextAttribsARBProc )( Display*, GLXFBConfig, GLXContext, Bool, const int* );
        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;

        {
            __GLXextFuncPtr fn = glXGetProcAddressARB( reinterpret_cast<const GLubyte*>( "glXCreateContextAttribsARB" ) );
            *reinterpret_cast<__GLXextFuncPtr*>( &glXCreateContextAttribsARB ) = fn;
        }

        if( nullptr == glXCreateContextAttribsARB )
        {
            ss_throw( "'glXCreateContextAttribsARB' procedure not found." );
        }

        const char* glxExts = glXQueryExtensionsString( d, DefaultScreen( d ) );

        if( false == isExtensionSupported( glxExts, "GLX_ARB_create_context" ) )
        {
            // Issue warning only as we seem to have gotten an address for the
            // function?
            ss_wrn( "'GLX_ARB_create_context' extension not supported." );
        }

        GLint attribs[] =
        {
            CONTEXT_MAJOR_VERSION_ARB, params.openGlVersion.first,  // 0,1
            CONTEXT_MINOR_VERSION_ARB, params.openGlVersion.second, // 2,3
            CONTEXT_FLAGS_ARB,         0,                           // 4,5
            0
        };

        if( params.openGlVersion.first >= 3 && params.allowOldOpenGL == false )
        {
            attribs[5] |= CONTEXT_FORWARD_COMPATIBLE_BIT_ARB; // No deprecated
            // features allowed.
        }

        if( params.enableDebugging )
        {
            attribs[5] |= CONTEXT_DEBUG_BIT_ARB; // Slower but logging internally
            // enabled in OpenGL.
        }

        // Install an X error handler so the application won't exit if GL 3.0
        // context allocation fails.
        //
        // Note this error handler is global.  All display connections in all
        // threads
        // of a process use the same error handler, so be sure to guard against
        // other
        // threads issuing X commands while this code is running.
        g_ctxErrorOccurred = false;
        int ( *oldHandler )( Display*, XErrorEvent* ) = XSetErrorHandler( &ctxErrorHandlerFn );

        const GLXContext sharedContext = 0;
        const Bool wantDirect = True;
        context_  = glXCreateContextAttribsARB( d, window_->best_config(), sharedContext, wantDirect, attribs );

        // Sync to ensure any errors generated are processed.
        XSync( d, False );

        // Restore previous handler.
        XSetErrorHandler( oldHandler );

        if( g_ctxErrorOccurred )
        {
            ss_err( "Error creating GLX ",
                    params.openGlVersion.first,
                    ".",
                    params.openGlVersion.second,
                    " context." );
            ss_throw( "Error creating GLX OpenGL context." );
        }

        if( 0 == context_ )
        {
            ss_err( "GLX ",
                    params.openGlVersion.first,
                    ".",
                    params.openGlVersion.second,
                    " context was zero." );
            ss_throw( " GLX OpenGL context was zero." );
        }
        else
        {
            ss_log( "Created GLX OpenGL ",
                    params.openGlVersion.first,
                    ".",
                    params.openGlVersion.second,
                    " context." );
        }

        // Verifying that context is a direct context.
        // Direct rendering contexts pass rendering commands directly from the calling process's
        // address space to the rendering system, bypassing the X server. Nondirect rendering
        // contexts pass all rendering commands to the X server.
        if( glXIsDirect( d, context_ ) )
        {
            ss_log( "Direct GLX rendering context obtained." );
        }
        else
        {
            ss_wrn( "Direct GLX rendering context obtained." );
        }

        glXMakeCurrent( d, *window_, context_ );

        assert( GL_NO_ERROR == glGetError() );

        const char* vendor      = reinterpret_cast<const char*>( glGetString( GL_VENDOR ) );
        assert( GL_NO_ERROR == glGetError() );
        const char* version     = reinterpret_cast<const char*>( glGetString( GL_VERSION ) );
        assert( GL_NO_ERROR == glGetError() );
        const char* renderer    = reinterpret_cast<const char*>( glGetString( GL_VENDOR ) );
        assert( GL_NO_ERROR == glGetError() );
        const char* language    = reinterpret_cast<const char*>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );
        assert( GL_NO_ERROR == glGetError() );

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

        if( false == enable_opengl_debugging( params.enableDebugging ) )
        {
            if( params.enableDebugging )
            {
                ss_wrn( "Unable to enable OpenGL debug output." );
            }
        }

        assert( GL_NO_ERROR == glGetError() );
    }

    x11windowopengl::~x11windowopengl()
    {
        if( window_ )
        {
            Display* d = window_->display();

            if( d && context_ )
            {
                glXMakeCurrent( d, 0, 0 );
                glXDestroyContext( d, context_ );
                context_ = 0;
            }
        }
    }

    //! Updates windowWidth_ and windowHeight_
    void x11windowopengl::handle_size_change( unsigned int newW, unsigned int newH )
    {
        ss_log( "(x11windowopengl)  handle_size_change() called." );
        ss_log( "newW = ", newW, " newH = ", newH );

        if( attached_window() )
        {
            attached_window()->on_resized( Dimension2d<unsigned int>( newW, newH ) );
            windowWidth_  = newW;
            windowHeight_ = newH;
        }
    }

    bool x11windowopengl::check_messages()
    {
        XEvent event;
        auto d = window_->display();

        char keybuffer[9] = { 0 };
        KeySym keysym;

        static const unsigned int buttonMap[] =
        {
            0xFFFF,                    CURSOR_PRIMARY_BUTTON_FLAG,
            CURSOR_MIDDLE_BUTTON_FLAG, CURSOR_SECONDARY_BUTTON_FLAG
        };

        while( XPending( d ) ) // loop to compress events
        {
            XNextEvent( d, &event );

            /*

            case WM_LBUTTONDBLCLK:
            input_changes().add_event(
            input_event::make_cursor_dbl_click(
            CURSOR_PRIMARY_BUTTON_FLAG | (wPrm & (MK_SHIFT|MK_CONTROL)),
            screenxy((uint32_t)lPrm)) );
            return 0;

            case WM_MOUSEMOVE:
            input_changes().add_event( input_event::make_cursor_move( (uint16_t)wPrm,
            screenxy((uint32_t)lPrm) ) );
            return 0;

            case WM_MOUSEWHEEL:
            input_changes().add_event( input_event::make_wheelmove(
            GET_WHEEL_DELTA_WPARAM(wPrm), screenxy((uint32_t)lPrm) ) );
            return 0;

            case WM_CLOSE:
            ss_log( "(windowswindowopengl)  WM_CLOSE" );
            continue_update_ = false;
            input_changes().shutdown();
            return 0;

            case WM_EXITSIZEMOVE:
            ss_log( "(windowswindowopengl)  WM_EXITSIZEMOVE message received." );
            handle_possible_size_change(false);
            break;

            case WM_SIZE:
            handle_possible_size_change(false);
            break;

            */
            switch( event.type )
            {
                case LeaveNotify:
                    ss_log( "Focus lost" );
                    alt_ctrl_shift_ = 0u;
                    input_changes().add_event( input_event::make_focus_lost() );
                    break;

                case EnterNotify:
                    ss_log( "Focus gained" );
                    alt_ctrl_shift_ = 0u;
                    input_changes().add_event( input_event::make_focus_gained() );
                    break;

                case FocusOut:
                    ss_log( "Focus lost" );
                    alt_ctrl_shift_ = 0u;
                    input_changes().add_event( input_event::make_focus_lost() );
                    break;

                case FocusIn:
                    ss_log( "Focus gained" );
                    alt_ctrl_shift_ = 0u;
                    input_changes().add_event( input_event::make_focus_gained() );
                    break;

                case ButtonPress:
                    if( event.xbutton.button > 0 && event.xbutton.button < 4 )
                    {
                        input_changes().add_event( input_event::make_cursor_button_down(
                                                       buttonMap[event.xbutton.button], screenxy( event.xbutton.x, event.xbutton.y ) ) );
                    }
                    else if( event.xbutton.button == 4 )
                    {
                        // X11 mouse up.
                        // A positive value indicates that the wheel was rotated
                        // forward, away from the user;
                        // a negative value indicates that the wheel was rotated
                        // backward, toward the user.
                        input_changes().add_event(
                            input_event::make_wheelmove( 120, screenxy( event.xbutton.x, event.xbutton.y ) ) );
                    }
                    else if( event.xbutton.button == 5 )
                    {
                        // X11 mouse down.
                        input_changes().add_event(
                            input_event::make_wheelmove( -120, screenxy( event.xbutton.x, event.xbutton.y ) ) );
                    }
                    break;

                case ButtonRelease:
                    if( event.xbutton.button < 4 )
                    {
                        input_changes().add_event( input_event::make_cursor_button_up(
                                                       buttonMap[event.xbutton.button], screenxy( event.xbutton.x, event.xbutton.y ) ) );

                        // Check for double clicks.
                        const bool doubleclicked = (mouse_button_released_id_ == event.xbutton.button) &&
                                                   (event.xbutton.time > mouse_release_time_) &&
                                                   ((event.xbutton.time - mouse_release_time_) >= DOUBLE_CLICK_MS);

                        if( doubleclicked )
                        {
                            ss_log( "DBL CLICK" );
                            input_changes().add_event( input_event::make_cursor_dbl_click(
                                                            buttonMap[event.xbutton.button], screenxy( event.xbutton.x, event.xbutton.y ) ) );

                            mouse_button_released_id_ = NOT_A_MOUSE_BUTTON_ID;
                            mouse_release_time_       = 0u;
                        }
                        else
                        {
                            // Record mouse button and event time.
                            mouse_button_released_id_ = event.xbutton.button;
                            mouse_release_time_       = event.xbutton.time;
                        }
                    }
                    break;

                case KeyPress:
                {
                    // We can grab/translate up to 8 chars (size of our buffer).
                    const auto bufferedChars = XLookupString( reinterpret_cast<XKeyEvent*>( &event ), keybuffer, 8, &keysym, NULL );

                    const auto key = translate_xkey_to_vkey( keysym );

                    if( VK_SHIFT == key || VK_LSHIFT == key || VK_RSHIFT == key )
                    {
                        alt_ctrl_shift_ |= input_event::shift_down;
                    }
                    else if( VK_CONTROL == key || VK_LCONTROL == key || VK_RCONTROL == key )
                    {
                        alt_ctrl_shift_ |= input_event::ctrl_down;
                    }
                    else if( VK_MENU == key )
                    {
                        alt_ctrl_shift_ |= input_event::alt_down;
                    }

                    ss_log( "KEY DOWN: ", key );
                    input_changes().add_event( input_event::make_key_down( key, alt_ctrl_shift_ ) );

                    if( bufferedChars == 1 )
                    {
                        ss_log( "KEY BUFFER 0:", static_cast<wchar_t>( keybuffer[0] ) );
                        input_changes().add_event( input_event::make_text_entry( static_cast<wchar_t>( keybuffer[0] ) ) );
                    }
                    else if( bufferedChars > 1 )
                    {
                        // UTF-8?
                        ss_wrn( "UTF-8? bufferedChars > 1" );
                    }
                }
                break;

                case KeyRelease:
                {
                    XLookupString( reinterpret_cast<XKeyEvent*>( &event ), keybuffer, 8, &keysym, NULL );

                    const auto key = translate_xkey_to_vkey( keysym );

                    if( VK_SHIFT == key || VK_LSHIFT == key || VK_RSHIFT == key )
                    {
                        alt_ctrl_shift_ &= ~input_event::shift_down;
                    }
                    else if( VK_CONTROL == key || VK_LCONTROL == key || VK_RCONTROL == key )
                    {
                        alt_ctrl_shift_ &= ~input_event::ctrl_down;
                    }
                    else if( VK_MENU == key )
                    {
                        alt_ctrl_shift_ &= ~input_event::alt_down;
                    }

                    input_changes().add_event( input_event::make_key_up( key, alt_ctrl_shift_ ) );
                }
                break;

                case MotionNotify:
                {
                    input_changes().add_event(
                        input_event::make_cursor_move( make_cursor_stateflags( event.xbutton.state ),
                                                       screenxy( event.xbutton.x, event.xbutton.y ) ) );
                }
                break;

                case ConfigureNotify:
                {
                    XConfigureEvent xce = event.xconfigure;

                    const auto newWidth  = static_cast<unsigned int>( xce.width );
                    const auto newHeight = static_cast<unsigned int>( xce.height );

                    // This event type is generated for a variety of
                    // happenings, so check whether the window has been
                    // resized.
                    if( newWidth != windowWidth_ || newHeight != windowHeight_ )
                    {
                        handle_size_change( newWidth, newHeight );
                    }
                }
                break;

                case ClientMessage:
                {
                    if( static_cast<unsigned int>(event.xclient.data.l[0]) == window_->del_win_atom() )
                    {
                        ss_log( "WM_DELETE_WINDOW client message." );
                        continue_update_ = false;
                        input_changes().shutdown();
                        break;
                    }
                }

                case Expose:
                    break;

                default:
                    ss_log( "Unknown/unhandled event type ", event.type );
                    break;
            }
        }

        // I think this is needed to ensure unhandled messages do not continue to
        // build up in the queue.
        XFlush( d );

        return continue_update_;
    }

    void x11windowopengl::on_attached( const std::shared_ptr<iwindow>& ,
                                       const std::shared_ptr<iwindow>& )
    {
        // NOP
    }

    bool x11windowopengl::is_shutdown() const
    {
        return !continue_update_;
    }

    void x11windowopengl::show()
    {
    }

    void x11windowopengl::set_as_fullscreen( const bool )
    {
    }

    bool x11windowopengl::is_fullscreen() const
    {
        return false;
    }

    bool x11windowopengl::enable_drag_and_drop( bool )
    {
        return false;
    }

    bool x11windowopengl::drag_and_drop_enabled() const
    {
        return false;
    }

    rect x11windowopengl::get_window_rect() const
    {
        XWindowAttributes xwAttr;
        auto d = window_->display();
        XGetWindowAttributes( d, *window_->get_window_ptr(), &xwAttr );
        return rect( 0, 0, dimension2dui( xwAttr.width, xwAttr.height ) );
    }

    rect x11windowopengl::get_screen_rect() const
    {
        auto d = window_->display();
        const Screen* pscr = DefaultScreenOfDisplay( d );
        ss_log( "Screen rect is ", pscr->width, " by ", pscr->height );
        return rect( 0, 0, dimension2dui( pscr->width, pscr->height ) );
    }

    const void* x11windowopengl::get_window_handle() const
    {
        return window_->get_window_ptr();
    }

    void x11windowopengl::swap_buffers()
    {
         glXSwapBuffers( window_->display(), *window_ );
         assert( GL_NO_ERROR == glGetError() );
    }

    void x11windowopengl::set_window_title( const std::string& )
    {
    }
}
