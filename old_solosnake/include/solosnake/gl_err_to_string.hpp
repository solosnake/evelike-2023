#ifndef SOLOSNAKE_GL_ERR_TO_STRING_HPP
#define SOLOSNAKE_GL_ERR_TO_STRING_HPP

namespace solosnake
{
    //! Pass an OpenGL error code in and get back an English language description of it.
    const char* gl_err_to_string( int err );
}

#endif 
