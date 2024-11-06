#ifndef solosnake_opengl_hpp
#define solosnake_opengl_hpp

#ifdef SS_OS_WINDOWS
#   define SOLOSNAKE_GL_OS_KNOWN 1
#   include "solosnake/win32/includewindows.hpp"
#   include <GL/gl.h>
#   include "solosnake/external/glext.h"
#else
#   ifndef SS_OS_LINUX
#       pragma message(__FILE__ " SS_OS_WINDOWS not defined.")
#   endif
#endif 

#ifdef SS_OS_LINUX
#   define SOLOSNAKE_GL_OS_KNOWN 1
#   define GL_GLEXT_PROTOTYPES
#   include <GL/glx.h> // this includes the necessary X headers
#   include <GL/gl.h>
#   include <GL/glext.h>
#   include <GL/glxext.h>
#endif

#ifndef SOLOSNAKE_GL_OS_KNOWN
#   include <GL/gl.h>
#   include <GL/glext.h>
#endif

/* Use C comments to silence multi-line comment warning.
//
//             +Y
//              |   -Z
//              | /
//              |/
//     -X ------+------- +X
//             /|
//            / |
//         +Z   |
//             -Y
//
//
//  Default front face (glFrontFace) is Counter Clockwise:
//
//
//            A  /\
//              /  \
//             /    \
//            /______\ C
//            B
*/

#endif
