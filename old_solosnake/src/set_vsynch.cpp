#include "solosnake/set_vsynch.hpp"

#ifdef SS_OS_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GL/glx.h> /* this includes the necessary X headers */
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glxext.h>
#endif

#ifdef SS_OS_WINDOWS
#include "solosnake/opengl.hpp"
#endif

namespace solosnake
{

    //! See http://www.opengl.org/registry/specs/EXT/wgl_swap_control.tx
    bool set_vsynch( const int interval )
    {
#ifdef __linux__
#define SS_GLX_LATE_SWAPS_TEAR_EXT 0x20F3

        auto fnglXSwapIntervalEXT = ( void ( * )( Display*, GLXDrawable, int ) )glXGetProcAddress(
                                        ( const GLubyte* )"glXSwapIntervalEXT" );
        if( fnglXSwapIntervalEXT )
        {
            Display* dpy = glXGetCurrentDisplay();
            GLXDrawable drawable = glXGetCurrentDrawable();
            if( dpy && drawable )
            {
                fnglXSwapIntervalEXT( dpy, drawable, interval );

                unsigned int lateSwapsEnabled = 0, currentInterval = 0xFF;

                glXQueryDrawable( dpy, drawable, SS_GLX_LATE_SWAPS_TEAR_EXT, &lateSwapsEnabled );
                glXQueryDrawable( dpy, drawable, GLX_SWAP_INTERVAL_EXT, &currentInterval );

                const bool requestedWasSet = ( interval == -1 )
                                             ? ( lateSwapsEnabled == 1u )
                                             : ( interval == static_cast<int>( currentInterval ) );

                return requestedWasSet;
            }
        }
#endif // __linux__

#ifdef SS_OS_WINDOWS
        // Use optional extension.
        typedef BOOL( APIENTRY * wglSwapIntervalEXT_t )( int interval );
        wglSwapIntervalEXT_t wglSwapIntervalEXT
            = ( wglSwapIntervalEXT_t )::wglGetProcAddress( "wglSwapIntervalEXT" );
        if( wglSwapIntervalEXT )
        {
            return TRUE == wglSwapIntervalEXT( interval );
        }
#endif // SS_OS_WINDOWS

        // Default:
        return false;
    }
}
