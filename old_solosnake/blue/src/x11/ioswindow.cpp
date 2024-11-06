#include <cassert>
#include <memory>
#include "solosnake/throw.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/get_opengl_version.hpp"
#include "solosnake/x11/window_opengl.hpp"

namespace solosnake
{
    namespace
    {
        //! Returns true if the string contains ' -d'. This is not a super strict
        //! analysis of the options string.
        static inline bool has_debug_in_options( const std::string& options )
        {
            return std::string::npos != options.find( " -d" );
        }

        static inline bool has_old_in_options( const std::string& options )
        {
            return std::string::npos != options.find( " -old" );
        }

        // Returns an x11windowopengl object.
        std::shared_ptr<ioswindow> openglwindow( const int opengl_major,
                                                 const int opengl_minor,
                                                 const unsigned int width,
                                                 const unsigned int height,
                                                 const bool allowResizing,
                                                 const std::string& title,
                                                 const bool fullscreen,
                                                 const std::string& /*classname*/,
                                                 const std::string& options )
        {
            const unsigned int zDepth  = 24u;
            const unsigned int stencil =  8u;

            x11windowopenglparams params;

            params.height  = height;
            params.width   = width;
            params.zDepth  = zDepth;
            params.stencil = stencil;
            params.allowOldOpenGL  = has_old_in_options( options );
            params.enableDebugging = has_debug_in_options( options );
            params.openGlVersion   = std::make_pair( opengl_major, opengl_minor );

            return std::make_shared<x11windowopengl>( title.c_str(), params );
        }
    }

    //! Creates a window of the specified width, height and with the specified
    //! settings. The type of window created depends on the string. By default an 
    //! OpenGL 2.1 window is created if the blank string is provided.
    //! Pass in "opengl X.Y", where X is a valid major release version and Y is a
    //! valid minor release version, to create an OpenGL window that supports that 
    //! version of OpenGL.
    //! Append ' -d' to get a debugging context for OpenGL.
    //! Versions including and later then 3.0 will be not be backwards compatible
    //! with deprecated features.
    //! This will throw if the requested OpenGL version is not supported on the
    //! current platform.
    std::shared_ptr<ioswindow> ioswindow::make_shared( const unsigned int width,
                                                       const unsigned int height,
                                                       const bool allowResizing,
                                                       const std::string& title,
                                                       const bool fullscreen,
                                                       const std::string& classname,
                                                       const std::string& options )
    {
        const auto version = get_opengl_version( options );

        const int opengl_major = version.first;
        const int opengl_minor = version.second;

        std::shared_ptr<ioswindow> w = openglwindow( opengl_major,
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
