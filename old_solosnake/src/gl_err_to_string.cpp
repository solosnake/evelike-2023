#include "solosnake/gl_err_to_string.hpp"
#include "solosnake/opengl.hpp"

namespace solosnake
{
    const char* gl_err_to_string( const int err )
    {
        switch( err )
        {
            case GL_NO_ERROR:
                return "GL_NO_ERROR";

            case GL_INVALID_ENUM:
                return "GL_INVALID_ENUM";

            case GL_INVALID_VALUE:
                return "GL_INVALID_VALUE";

            case GL_INVALID_OPERATION:
                return "GL_INVALID_OPERATION";

            case GL_STACK_OVERFLOW:
                return "GL_STACK_OVERFLOW";

            case GL_STACK_UNDERFLOW:
                return "GL_STACK_UNDERFLOW";

            case GL_OUT_OF_MEMORY:
                return "GL_OUT_OF_MEMORY";

            default:
                break;
        }

        return "Unknown OpenGL error flag.";
    }
}
