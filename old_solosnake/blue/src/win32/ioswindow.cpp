#include <memory>
#include "solosnake/throw.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/get_opengl_version.hpp"
#include "solosnake/win32/window_opengl.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        //! Returns true if the string contains ' -d'. This is not a super strict
        //! analysis of the options string.
        static inline bool has_debug_in_options( const string& options )
        {
            return string::npos != options.find( " -d" );
        }

        static inline bool has_old_in_options( const string& options )
        {
            return string::npos != options.find( " -old" );
        }
    }

    shared_ptr<ioswindow> openglwindow( const int opengl_major,
                                        const int opengl_minor,
                                        const unsigned int width,
                                        const unsigned int height,
                                        const bool allowResizing,
                                        const string& title,
                                        const bool fullscreen,
                                        const string& classname,
                                        const string& options )
    {
        const unsigned int zDepth = 24u;
        const unsigned int fsAA   =  0u;

        windowswindowopengl::params init;
        init.title                 = title;
        init.width                 = width;
        init.height                = height;
        init.openGlVersion         = make_pair( opengl_major, opengl_minor );
        init.allowDeprecatedOpenGL = has_old_in_options( options );
        init.enableDebugging       = has_debug_in_options( options );
        init.fullscreencursor      = true;
        init.fullscreen            = fullscreen;
        init.allowResizing         = allowResizing;
        init.zDepth                = zDepth;
        init.fsaa                  = fsAA;
        init.classname             = classname;

        return make_shared<windowswindowopengl>( init );
    }

    //! Creates a window with the specified settings. If not fullscreen the window will occupy
    //! 25% of the primary screen and be centred.
    //! The type of window created depends on the contents of the options string. By default
    //! an OpenGL 2.1 window is created if the blank string is provided.
    //! Pass in "opengl X.Y", where X is a valid major release version and Y is a valid minor
    //! release version, to create an OpenGL window that supports that version of OpenGL.
    //! Append ' -d' to get a debugging context for OpenGL.
    //! Versions including and later then 3.0 will be not be backwards compatible with deprecated
    //! features.
    //! This will throw if the requested OpenGL version is not supported on the current platform.
    //! Creates a window with an OpenGL context.
    //! @param 
    shared_ptr<ioswindow> ioswindow::make_shared( const unsigned int width,
                                                  const unsigned int height,
                                                  const bool allowResizing,
                                                  const string& title,
                                                  const bool fullscreen,
                                                  const string& classname,
                                                  const string& options )
    {
        const auto version = get_opengl_version( options );

        const int opengl_major = version.first;
        const int opengl_minor = version.second;

        shared_ptr<ioswindow> w = openglwindow( opengl_major,
                                                opengl_minor,
                                                width,
                                                height,
                                                allowResizing,
                                                title,
                                                fullscreen,
                                                classname,
                                                options );

        return w;
    }
}
