#ifndef solosnake_get_opengl_version_hpp
#define solosnake_get_opengl_version_hpp

#include <string>
#include <utility>

namespace solosnake
{

    //! Returns an OpenGL version from the string. If the string just says
    //! "opengl" and no version, version 2.1 is returned.
    //! Returns 1.0, 1.1,. 1.2, 1.3, 1.4, 1.5, 2.0, 2.1, 3.0, 3.1, 3.2, 3.3,
    //! or 4.n.
    //! The string is assumed to be an options string and can contain other data.
    //! The opengl version must be specified as: "opengl" or "opengl X.Y".
    std::pair<int, int> get_opengl_version( const std::string& options );
}

#endif
