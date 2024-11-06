//! This file declares and makes available the OpenGL extensions required by the deferred_renderer. On some
//! platforms these may be provided automatically. On Microsoft Windows these are extensions loaded
//! through an existing OpenGL context. They are not declared inside any namespace.
//! If your platform requires some setup or initialisation of the extensions, define the
//! SS_INIT_OPENGL_EXTENSIONS so as to point to a function call which will be called by the deferred_renderer
//! prior to any usage of non core OpenGL functions. This call can in theory be called several times
//! inside an application, once per deferred_renderer object.
//! To use the GLEW ( http://glew.sourceforge.net/basic.html ) as a source of extensions, when using
//! Windows, define SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW


//#   define SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW

//#ifdef __linux__
//#   ifndef SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW
//#       define SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW     (1)
//#   endif
//#endif

#ifdef  SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW
#pragma message("SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW")
#   define SS_DEFERRED_RENDERER_OPENGL_EXTENSIONS
#   define SS_INIT_OPENGL_EXTENSIONS                       init_glew

#   ifdef SS_OS_LINUX
#       define GL_GLEXT_PROTOTYPES
#       include <GL/glew.h>
#       include <GL/glx.h>    /* this includes the necessary X headers */
#       include <GL/gl.h>
#       include <GL/glext.h>
#       include <GL/glxext.h>
#   endif

#   ifdef SS_OS_WINDOWS
#pragma message("SS_OS_WINDOWS")
//#       include "solosnake/external/glew.hpp"
#       include "solosnake/opengl.hpp"
#   endif


namespace
{
    void init_glew()
    {
        if( GL_NO_ERROR != glGetError() )
        {
            ss_err( "OpenGL error code set prior to initialising GLEW" );
        }

        ss_log( "Initialising GLEW" );
        // Set the global 'glewExperimental' to true to allow us to load
        // extensions even inside a new context (which deprecated the old
        // extension lookup.
        glewExperimental = true;
        GLenum err = glewInit();
        if( GLEW_OK != err )
        {
            ss_err( "Unable to initialise GLEW." );
            ss_err( glewGetErrorString( err ) );
            ss_throw( "Unable to initialise GLEW." );
        }
        else
        {
            if( GL_NO_ERROR != glGetError() )
            {
                ss_wrn( "OpenGL error code set after initialising GLEW" );
            }
        }
    }
}

#else

#pragma message("NO SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW")

#   ifdef SS_OS_LINUX
#       pragma message("SS_OS_LINUX using GLX OpenGL prototypes.")
#       define SS_DEFERRED_RENDERER_OPENGL_EXTENSIONS
#       define SS_INIT_OPENGL_EXTENSIONS()                  /* empty */
#       define GL_GLEXT_PROTOTYPES
#       include <GL/glx.h>    /* this includes the necessary X headers */
#       include <GL/glxext.h>
#   endif

#   ifdef SS_OS_WINDOWS
#       pragma message("SS_OS_WINDOWS using solosnake GL prototypes.")
#       define SS_DEFERRED_RENDERER_OPENGL_EXTENSIONS
#       include "solosnake/opengl.hpp"
#       define SS_INIT_OPENGL_EXTENSIONS                    init_OpenGL_extensions_Win32
#       define SS_GLAPI                                     APIENTRY*
#       define SS_CONCAT( a, b )                            a##b
#       define SS_LOAD_OPENGL_EXTENSION( F )                F = ( SS_CONCAT( f, F ) ) getOpenGLProcAddress( #F )
#       define SS_TRY_LOAD_OPENGL_EXTENSION( F )            F = ( SS_CONCAT( f, F ) ) tryGetOpenGLProcAddress( #F )

namespace
{
    // OpenGL function pointers.
    extern "C"
    {
        // Typedefs (required for casting).
        typedef GLenum      (SS_GLAPI  fglCheckFramebufferStatus      )   (GLenum);
        typedef void        (SS_GLAPI  fglBindVertexArray             )   (GLuint array);
        typedef void        (SS_GLAPI  fglDeleteVertexArrays          )   (GLsizei n, const GLuint *arrays);
        typedef void        (SS_GLAPI  fglGenVertexArrays             )   (GLsizei n, GLuint *arrays);
        typedef GLboolean   (SS_GLAPI  fglIsVertexArray               )   (GLuint array);
        typedef void        (SS_GLAPI  fglBlendEquationSeparate       )   (GLenum modeRGB, GLenum modeAlpha);
        typedef void        (SS_GLAPI  fglDrawBuffers                 )   (GLsizei n, const GLenum *bufs);
        typedef void        (SS_GLAPI  fglStencilOpSeparate           )   (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
        typedef void        (SS_GLAPI  fglStencilFuncSeparate         )   (GLenum face, GLenum func, GLint ref, GLuint mask);
        typedef void        (SS_GLAPI  fglStencilMaskSeparate         )   (GLenum face, GLuint mask);
        typedef void        (SS_GLAPI  fglAttachShader                )   (GLuint program, GLuint shader);
        typedef void        (SS_GLAPI  fglBindAttribLocation          )   (GLuint program, GLuint index, const GLchar *name);
        typedef void        (SS_GLAPI  fglCompileShader               )   (GLuint shader);
        typedef GLuint      (SS_GLAPI  fglCreateProgram               )   (void);
        typedef GLuint      (SS_GLAPI  fglCreateShader                )   (GLenum type);
        typedef void        (SS_GLAPI  fglDeleteProgram               )   (GLuint program);
        typedef void        (SS_GLAPI  fglDeleteShader                )   (GLuint shader);
        typedef void        (SS_GLAPI  fglDetachShader                )   (GLuint program, GLuint shader);
        typedef void        (SS_GLAPI  fglDisableVertexAttribArray    )   (GLuint index);
        typedef void        (SS_GLAPI  fglEnableVertexAttribArray     )   (GLuint index);
        typedef void        (SS_GLAPI  fglGetActiveAttrib             )   (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
        typedef void        (SS_GLAPI  fglGetActiveUniform            )   (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
        typedef void        (SS_GLAPI  fglGetAttachedShaders          )   (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj);
        typedef GLint       (SS_GLAPI  fglGetAttribLocation           )   (GLuint program, const GLchar *name);
        typedef void        (SS_GLAPI  fglGetProgramiv                )   (GLuint program, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetProgramInfoLog           )   (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
        typedef void        (SS_GLAPI  fglGetShaderiv                 )   (GLuint shader, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetShaderInfoLog            )   (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
        typedef void        (SS_GLAPI  fglGetShaderSource             )   (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
        typedef GLint       (SS_GLAPI  fglGetUniformLocation          )   (GLuint program, const GLchar *name);
        typedef void        (SS_GLAPI  fglGetUniformfv                )   (GLuint program, GLint location, GLfloat *params);
        typedef void        (SS_GLAPI  fglGetUniformiv                )   (GLuint program, GLint location, GLint *params);
        typedef void        (SS_GLAPI  fglGetVertexAttribdv           )   (GLuint index, GLenum pname, GLdouble *params);
        typedef void        (SS_GLAPI  fglGetVertexAttribfv           )   (GLuint index, GLenum pname, GLfloat *params);
        typedef void        (SS_GLAPI  fglGetVertexAttribiv           )   (GLuint index, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetVertexAttribPointerv     )   (GLuint index, GLenum pname, GLvoid* *pointer);
        typedef GLboolean   (SS_GLAPI  fglIsProgram                   )   (GLuint program);
        typedef GLboolean   (SS_GLAPI  fglIsShader                    )   (GLuint shader);
        typedef void        (SS_GLAPI  fglLinkProgram                 )   (GLuint program);
        typedef void        (SS_GLAPI  fglShaderSource                )   (GLuint shader, GLsizei count, const GLchar* *string, const GLint *length);
        typedef void        (SS_GLAPI  fglUseProgram                  )   (GLuint program);
        typedef void        (SS_GLAPI  fglUniform1f                   )   (GLint location, GLfloat v0);
        typedef void        (SS_GLAPI  fglUniform2f                   )   (GLint location, GLfloat v0, GLfloat v1);
        typedef void        (SS_GLAPI  fglUniform3f                   )   (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
        typedef void        (SS_GLAPI  fglUniform4f                   )   (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
        typedef void        (SS_GLAPI  fglUniform1ui                  )   (GLint location, GLuint v0);
        typedef void        (SS_GLAPI  fglUniform1i                   )   (GLint location, GLint v0);
        typedef void        (SS_GLAPI  fglUniform2i                   )   (GLint location, GLint v0, GLint v1);
        typedef void        (SS_GLAPI  fglUniform3i                   )   (GLint location, GLint v0, GLint v1, GLint v2);
        typedef void        (SS_GLAPI  fglUniform4i                   )   (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
        typedef void        (SS_GLAPI  fglUniform1fv                  )   (GLint location, GLsizei count, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniform2fv                  )   (GLint location, GLsizei count, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniform3fv                  )   (GLint location, GLsizei count, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniform4fv                  )   (GLint location, GLsizei count, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniform1iv                  )   (GLint location, GLsizei count, const GLint *value);
        typedef void        (SS_GLAPI  fglUniform2iv                  )   (GLint location, GLsizei count, const GLint *value);
        typedef void        (SS_GLAPI  fglUniform3iv                  )   (GLint location, GLsizei count, const GLint *value);
        typedef void        (SS_GLAPI  fglUniform4iv                  )   (GLint location, GLsizei count, const GLint *value);
        typedef void        (SS_GLAPI  fglUniformMatrix2fv            )   (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniformMatrix3fv            )   (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        typedef void        (SS_GLAPI  fglUniformMatrix4fv            )   (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
        typedef void        (SS_GLAPI  fglValidateProgram             )   (GLuint program);
        typedef void        (SS_GLAPI  fglGenQueries                  )   (GLsizei n, GLuint *ids);
        typedef void        (SS_GLAPI  fglDeleteQueries               )   (GLsizei n, const GLuint *ids);
        typedef GLboolean   (SS_GLAPI  fglIsQuery                     )   (GLuint id);
        typedef void        (SS_GLAPI  fglBeginQuery                  )   (GLenum target, GLuint id);
        typedef void        (SS_GLAPI  fglEndQuery                    )   (GLenum target);
        typedef void        (SS_GLAPI  fglGetQueryiv                  )   (GLenum target, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetQueryObjectiv            )   (GLuint id, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetQueryObjectuiv           )   (GLuint id, GLenum pname, GLuint *params);
        typedef void        (SS_GLAPI  fglBindBuffer                  )   (GLenum target, GLuint buffer);
        typedef void        (SS_GLAPI  fglDeleteBuffers               )   (GLsizei n, const GLuint *buffers);
        typedef void        (SS_GLAPI  fglGenBuffers                  )   (GLsizei n, GLuint *buffers);
        typedef GLboolean   (SS_GLAPI  fglIsBuffer                    )   (GLuint buffer);
        typedef void        (SS_GLAPI  fglBufferData                  )   (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
        typedef void        (SS_GLAPI  fglBufferSubData               )   (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
        typedef void        (SS_GLAPI  fglGetBufferSubData            )   (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
        typedef GLvoid*     (SS_GLAPI  fglMapBuffer                   )   (GLenum target, GLenum access);
        typedef GLboolean   (SS_GLAPI  fglUnmapBuffer                 )   (GLenum target);
        typedef void        (SS_GLAPI  fglGetBufferParameteriv        )   (GLenum target, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetBufferPointerv           )   (GLenum target, GLenum pname, GLvoid* *params);
        typedef void        (SS_GLAPI  fglVertexAttribDivisor         )   (GLuint index, GLuint divisor);
        typedef void        (SS_GLAPI  fglVertexAttribPointer         )   (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
        typedef void        (SS_GLAPI  fglGenerateMipmap              )   (GLenum target);
        typedef void        (SS_GLAPI  fglActiveTexture               )   (GLenum texture);
        typedef GLboolean   (SS_GLAPI  fglIsFramebuffer               )   (GLuint framebuffer);
        typedef void        (SS_GLAPI  fglBindFramebuffer             )   (GLenum target, GLuint framebuffer);
        typedef void        (SS_GLAPI  fglDeleteFramebuffers          )   (GLsizei n, const GLuint *framebuffers);
        typedef void        (SS_GLAPI  fglGenFramebuffers             )   (GLsizei n, GLuint *framebuffers);
        typedef void        (SS_GLAPI  fglFramebufferTexture2D        )   (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
        typedef void        (SS_GLAPI  fglBindFragDataLocation        )   (GLuint program, GLuint color, const GLchar *name);
        typedef void        (SS_GLAPI  fglGetUniformIndices           )   (GLuint program, GLsizei uniformCount, const GLchar* *uniformNames, GLuint *uniformIndices);
        typedef void        (SS_GLAPI  fglGetActiveUniformsiv         )   (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetActiveUniformName        )   (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
        typedef GLuint      (SS_GLAPI  fglGetUniformBlockIndex        )   (GLuint program, const GLchar *uniformBlockName);
        typedef void        (SS_GLAPI  fglGetActiveUniformBlockiv     )   (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
        typedef void        (SS_GLAPI  fglGetActiveUniformBlockName   )   (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
        typedef void        (SS_GLAPI  fglUniformBlockBinding         )   (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
        typedef void        (SS_GLAPI  fglBindBufferRange             )   (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
        typedef void        (SS_GLAPI  fglBindBufferBase              )   (GLenum target, GLuint index, GLuint buffer);
        typedef void        (SS_GLAPI  fglDrawArraysInstanced         )   (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
        typedef void        (SS_GLAPI  fglDrawElementsInstanced       )   (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount);
        typedef void        (SS_GLAPI  fglTexBuffer                   )   (GLenum target, GLenum internalformat, GLuint buffer);
        typedef void        (SS_GLAPI  fglTexImage3D                  )   (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
        typedef void        (SS_GLAPI  fglPrimitiveRestartIndex       )   (GLuint index);
        typedef void        (SS_GLAPI  fglBlitFramebuffer             )   (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
        typedef void        (SS_GLAPI  fglBlendFunci                  )   (GLuint buf, GLenum src, GLenum dst);
        typedef void        (SS_GLAPI  fglBlendFuncSeparatei          )   (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

        // Function pointer declarations:
        static fglDeleteBuffers                glDeleteBuffers                 = nullptr;
        static fglBindVertexArray              glBindVertexArray               = nullptr;
        static fglDeleteVertexArrays           glDeleteVertexArrays            = nullptr;
        static fglGenVertexArrays              glGenVertexArrays               = nullptr;
        static fglIsVertexArray                glIsVertexArray                 = nullptr;
        static fglBlendEquationSeparate        glBlendEquationSeparate         = nullptr;
        static fglDrawBuffers                  glDrawBuffers                   = nullptr;
        static fglStencilOpSeparate            glStencilOpSeparate             = nullptr;
        static fglStencilFuncSeparate          glStencilFuncSeparate           = nullptr;
        static fglStencilMaskSeparate          glStencilMaskSeparate           = nullptr;
        static fglAttachShader                 glAttachShader                  = nullptr;
        static fglBindAttribLocation           glBindAttribLocation            = nullptr;
        static fglCompileShader                glCompileShader                 = nullptr;
        static fglCreateProgram                glCreateProgram                 = nullptr;
        static fglCreateShader                 glCreateShader                  = nullptr;
        static fglDeleteProgram                glDeleteProgram                 = nullptr;
        static fglDeleteShader                 glDeleteShader                  = nullptr;
        static fglDetachShader                 glDetachShader                  = nullptr;
        static fglDisableVertexAttribArray     glDisableVertexAttribArray      = nullptr;
        static fglEnableVertexAttribArray      glEnableVertexAttribArray       = nullptr;
        static fglGetActiveAttrib              glGetActiveAttrib               = nullptr;
        static fglGetActiveUniform             glGetActiveUniform              = nullptr;
        static fglGetAttachedShaders           glGetAttachedShaders            = nullptr;
        static fglGetAttribLocation            glGetAttribLocation             = nullptr;
        static fglGetProgramiv                 glGetProgramiv                  = nullptr;
        static fglGetProgramInfoLog            glGetProgramInfoLog             = nullptr;
        static fglGetShaderiv                  glGetShaderiv                   = nullptr;
        static fglGetShaderInfoLog             glGetShaderInfoLog              = nullptr;
        static fglGetShaderSource              glGetShaderSource               = nullptr;
        static fglGetUniformLocation           glGetUniformLocation            = nullptr;
        static fglGetUniformfv                 glGetUniformfv                  = nullptr;
        static fglGetUniformiv                 glGetUniformiv                  = nullptr;
        static fglGetVertexAttribdv            glGetVertexAttribdv             = nullptr;
        static fglGetVertexAttribfv            glGetVertexAttribfv             = nullptr;
        static fglGetVertexAttribiv            glGetVertexAttribiv             = nullptr;
        static fglGetVertexAttribPointerv      glGetVertexAttribPointerv       = nullptr;
        static fglIsProgram                    glIsProgram                     = nullptr;
        static fglIsShader                     glIsShader                      = nullptr;
        static fglLinkProgram                  glLinkProgram                   = nullptr;
        static fglShaderSource                 glShaderSource                  = nullptr;
        static fglUseProgram                   glUseProgram                    = nullptr;
        static fglUniform1f                    glUniform1f                     = nullptr;
        static fglUniform2f                    glUniform2f                     = nullptr;
        static fglUniform3f                    glUniform3f                     = nullptr;
        static fglUniform4f                    glUniform4f                     = nullptr;
        static fglUniform1ui                   glUniform1ui                    = nullptr;
        static fglUniform1i                    glUniform1i                     = nullptr;
        static fglUniform2i                    glUniform2i                     = nullptr;
        static fglUniform3i                    glUniform3i                     = nullptr;
        static fglUniform4i                    glUniform4i                     = nullptr;
        static fglUniform1fv                   glUniform1fv                    = nullptr;
        static fglUniform2fv                   glUniform2fv                    = nullptr;
        static fglUniform3fv                   glUniform3fv                    = nullptr;
        static fglUniform4fv                   glUniform4fv                    = nullptr;
        static fglUniform1iv                   glUniform1iv                    = nullptr;
        static fglUniform2iv                   glUniform2iv                    = nullptr;
        static fglUniform3iv                   glUniform3iv                    = nullptr;
        static fglUniform4iv                   glUniform4iv                    = nullptr;
        static fglUniformMatrix2fv             glUniformMatrix2fv              = nullptr;
        static fglUniformMatrix3fv             glUniformMatrix3fv              = nullptr;
        static fglUniformMatrix4fv             glUniformMatrix4fv              = nullptr;
        static fglValidateProgram              glValidateProgram               = nullptr;
        static fglGenQueries                   glGenQueries                    = nullptr;
        static fglDeleteQueries                glDeleteQueries                 = nullptr;
        static fglIsQuery                      glIsQuery                       = nullptr;
        static fglBeginQuery                   glBeginQuery                    = nullptr;
        static fglEndQuery                     glEndQuery                      = nullptr;
        static fglGetQueryiv                   glGetQueryiv                    = nullptr;
        static fglGetQueryObjectiv             glGetQueryObjectiv              = nullptr;
        static fglGetQueryObjectuiv            glGetQueryObjectuiv             = nullptr;
        static fglBindBuffer                   glBindBuffer                    = nullptr;
        static fglGenBuffers                   glGenBuffers                    = nullptr;
        static fglIsBuffer                     glIsBuffer                      = nullptr;
        static fglBufferData                   glBufferData                    = nullptr;
        static fglBufferSubData                glBufferSubData                 = nullptr;
        static fglGetBufferSubData             glGetBufferSubData              = nullptr;
        static fglMapBuffer                    glMapBuffer                     = nullptr;
        static fglUnmapBuffer                  glUnmapBuffer                   = nullptr;
        static fglGetBufferParameteriv         glGetBufferParameteriv          = nullptr;
        static fglGetBufferPointerv            glGetBufferPointerv             = nullptr;
        static fglVertexAttribDivisor          glVertexAttribDivisor           = nullptr;
        static fglVertexAttribPointer          glVertexAttribPointer           = nullptr;
        static fglGenerateMipmap               glGenerateMipmap                = nullptr;
        static fglActiveTexture                glActiveTexture                 = nullptr;
        static fglIsFramebuffer                glIsFramebuffer                 = nullptr;
        static fglBindFramebuffer              glBindFramebuffer               = nullptr;
        static fglGenFramebuffers              glGenFramebuffers               = nullptr;
        static fglDeleteFramebuffers           glDeleteFramebuffers            = nullptr;
        static fglCheckFramebufferStatus       glCheckFramebufferStatus        = nullptr;
        static fglFramebufferTexture2D         glFramebufferTexture2D          = nullptr;
        static fglBindFragDataLocation         glBindFragDataLocation          = nullptr;
        static fglGetUniformIndices            glGetUniformIndices             = nullptr;
        static fglGetActiveUniformsiv          glGetActiveUniformsiv           = nullptr;
        static fglGetActiveUniformName         glGetActiveUniformName          = nullptr;
        static fglGetUniformBlockIndex         glGetUniformBlockIndex          = nullptr;
        static fglGetActiveUniformBlockiv      glGetActiveUniformBlockiv       = nullptr;
        static fglGetActiveUniformBlockName    glGetActiveUniformBlockName     = nullptr;
        static fglUniformBlockBinding          glUniformBlockBinding           = nullptr;
        static fglBindBufferRange              glBindBufferRange               = nullptr;
        static fglBindBufferBase               glBindBufferBase                = nullptr;
        static fglDrawArraysInstanced          glDrawArraysInstanced           = nullptr;
        static fglDrawElementsInstanced        glDrawElementsInstanced         = nullptr;
        static fglTexBuffer                    glTexBuffer                     = nullptr;
        static fglTexImage3D                   glTexImage3D                    = nullptr;
        static fglPrimitiveRestartIndex        glPrimitiveRestartIndex         = nullptr;
        static fglBlitFramebuffer              glBlitFramebuffer               = nullptr;
        static fglBlendFunci                   glBlendFunci                    = nullptr;
        static fglBlendFuncSeparatei           glBlendFuncSeparatei            = nullptr;
    }
}

PROC tryGetOpenGLProcAddress( LPCSTR fname )
{
    auto f = ::wglGetProcAddress( fname );

    if( nullptr == f )
    {
        ss_log( "Unable to find address of OpenGL extension '", fname );
    }

    return f;
}

PROC getOpenGLProcAddress( LPCSTR fname )
{
    auto f = tryGetOpenGLProcAddress( fname );

    if( nullptr == f )
    {
        ss_throw( "Missing OpenGL extension." );
    }

    return f;
}


void init_optional_OpenGL_extensions_Win32()
{
}

void init_required_OpenGL_extensions_Win32()
{
    ss_log( "Getting required OpenGL functions." );
    SS_LOAD_OPENGL_EXTENSION( glDeleteBuffers );
    SS_LOAD_OPENGL_EXTENSION( glBindVertexArray );
    SS_LOAD_OPENGL_EXTENSION( glDeleteVertexArrays );
    SS_LOAD_OPENGL_EXTENSION( glGenVertexArrays );
    SS_LOAD_OPENGL_EXTENSION( glIsVertexArray );
    SS_LOAD_OPENGL_EXTENSION( glBlendEquationSeparate );
    SS_LOAD_OPENGL_EXTENSION( glDrawBuffers );
    SS_LOAD_OPENGL_EXTENSION( glStencilOpSeparate );
    SS_LOAD_OPENGL_EXTENSION( glStencilFuncSeparate );
    SS_LOAD_OPENGL_EXTENSION( glStencilMaskSeparate );
    SS_LOAD_OPENGL_EXTENSION( glAttachShader );
    SS_LOAD_OPENGL_EXTENSION( glBindAttribLocation );
    SS_LOAD_OPENGL_EXTENSION( glCompileShader );
    SS_LOAD_OPENGL_EXTENSION( glCreateProgram );
    SS_LOAD_OPENGL_EXTENSION( glCreateShader );
    SS_LOAD_OPENGL_EXTENSION( glDeleteProgram );
    SS_LOAD_OPENGL_EXTENSION( glDeleteShader );
    SS_LOAD_OPENGL_EXTENSION( glDetachShader );
    SS_LOAD_OPENGL_EXTENSION( glDisableVertexAttribArray );
    SS_LOAD_OPENGL_EXTENSION( glEnableVertexAttribArray );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveAttrib );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveUniform );
    SS_LOAD_OPENGL_EXTENSION( glGetAttachedShaders );
    SS_LOAD_OPENGL_EXTENSION( glGetAttribLocation );
    SS_LOAD_OPENGL_EXTENSION( glGetProgramiv );
    SS_LOAD_OPENGL_EXTENSION( glGetProgramInfoLog );
    SS_LOAD_OPENGL_EXTENSION( glGetShaderiv );
    SS_LOAD_OPENGL_EXTENSION( glGetShaderInfoLog );
    SS_LOAD_OPENGL_EXTENSION( glGetShaderSource );
    SS_LOAD_OPENGL_EXTENSION( glGetUniformLocation );
    SS_LOAD_OPENGL_EXTENSION( glGetUniformfv );
    SS_LOAD_OPENGL_EXTENSION( glGetUniformiv );
    SS_LOAD_OPENGL_EXTENSION( glGetVertexAttribdv );
    SS_LOAD_OPENGL_EXTENSION( glGetVertexAttribfv );
    SS_LOAD_OPENGL_EXTENSION( glGetVertexAttribiv );
    SS_LOAD_OPENGL_EXTENSION( glGetVertexAttribPointerv );
    SS_LOAD_OPENGL_EXTENSION( glIsProgram );
    SS_LOAD_OPENGL_EXTENSION( glIsShader );
    SS_LOAD_OPENGL_EXTENSION( glLinkProgram );
    SS_LOAD_OPENGL_EXTENSION( glShaderSource );
    SS_LOAD_OPENGL_EXTENSION( glUseProgram );
    SS_LOAD_OPENGL_EXTENSION( glUniform1f );
    SS_LOAD_OPENGL_EXTENSION( glUniform2f );
    SS_LOAD_OPENGL_EXTENSION( glUniform3f );
    SS_LOAD_OPENGL_EXTENSION( glUniform4f );
    SS_LOAD_OPENGL_EXTENSION( glUniform1ui );
    SS_LOAD_OPENGL_EXTENSION( glUniform1i );
    SS_LOAD_OPENGL_EXTENSION( glUniform2i );
    SS_LOAD_OPENGL_EXTENSION( glUniform3i );
    SS_LOAD_OPENGL_EXTENSION( glUniform4i );
    SS_LOAD_OPENGL_EXTENSION( glUniform1fv );
    SS_LOAD_OPENGL_EXTENSION( glUniform2fv );
    SS_LOAD_OPENGL_EXTENSION( glUniform3fv );
    SS_LOAD_OPENGL_EXTENSION( glUniform4fv );
    SS_LOAD_OPENGL_EXTENSION( glUniform1iv );
    SS_LOAD_OPENGL_EXTENSION( glUniform2iv );
    SS_LOAD_OPENGL_EXTENSION( glUniform3iv );
    SS_LOAD_OPENGL_EXTENSION( glUniform4iv );
    SS_LOAD_OPENGL_EXTENSION( glUniformMatrix2fv );
    SS_LOAD_OPENGL_EXTENSION( glUniformMatrix3fv );
    SS_LOAD_OPENGL_EXTENSION( glUniformMatrix4fv );
    SS_LOAD_OPENGL_EXTENSION( glValidateProgram );
    SS_LOAD_OPENGL_EXTENSION( glGenQueries );
    SS_LOAD_OPENGL_EXTENSION( glDeleteQueries );
    SS_LOAD_OPENGL_EXTENSION( glIsQuery );
    SS_LOAD_OPENGL_EXTENSION( glBeginQuery );
    SS_LOAD_OPENGL_EXTENSION( glEndQuery );
    SS_LOAD_OPENGL_EXTENSION( glGetQueryiv );
    SS_LOAD_OPENGL_EXTENSION( glGetQueryObjectiv );
    SS_LOAD_OPENGL_EXTENSION( glGetQueryObjectuiv );
    SS_LOAD_OPENGL_EXTENSION( glBindBuffer );
    SS_LOAD_OPENGL_EXTENSION( glGenBuffers );
    SS_LOAD_OPENGL_EXTENSION( glIsBuffer );
    SS_LOAD_OPENGL_EXTENSION( glBufferData );
    SS_LOAD_OPENGL_EXTENSION( glBufferSubData );
    SS_LOAD_OPENGL_EXTENSION( glGetBufferSubData );
    SS_LOAD_OPENGL_EXTENSION( glMapBuffer );
    SS_LOAD_OPENGL_EXTENSION( glUnmapBuffer );
    SS_LOAD_OPENGL_EXTENSION( glGetBufferParameteriv );
    SS_LOAD_OPENGL_EXTENSION( glGetBufferPointerv );
    SS_LOAD_OPENGL_EXTENSION( glVertexAttribDivisor );
    SS_LOAD_OPENGL_EXTENSION( glVertexAttribPointer );
    SS_LOAD_OPENGL_EXTENSION( glGenerateMipmap );
    SS_LOAD_OPENGL_EXTENSION( glActiveTexture );
    SS_LOAD_OPENGL_EXTENSION( glIsFramebuffer );
    SS_LOAD_OPENGL_EXTENSION( glBindFramebuffer );
    SS_LOAD_OPENGL_EXTENSION( glGenFramebuffers );
    SS_LOAD_OPENGL_EXTENSION( glDeleteFramebuffers );
    SS_LOAD_OPENGL_EXTENSION( glCheckFramebufferStatus );
    SS_LOAD_OPENGL_EXTENSION( glFramebufferTexture2D );
    SS_LOAD_OPENGL_EXTENSION( glBindFragDataLocation );
    SS_LOAD_OPENGL_EXTENSION( glGetUniformIndices );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveUniformsiv );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveUniformName );
    SS_LOAD_OPENGL_EXTENSION( glGetUniformBlockIndex );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveUniformBlockiv );
    SS_LOAD_OPENGL_EXTENSION( glGetActiveUniformBlockName );
    SS_LOAD_OPENGL_EXTENSION( glUniformBlockBinding );
    SS_LOAD_OPENGL_EXTENSION( glBindBufferRange );
    SS_LOAD_OPENGL_EXTENSION( glBindBufferBase );
    SS_LOAD_OPENGL_EXTENSION( glDrawArraysInstanced );
    SS_LOAD_OPENGL_EXTENSION( glDrawElementsInstanced );
    SS_LOAD_OPENGL_EXTENSION( glTexBuffer );
    SS_LOAD_OPENGL_EXTENSION( glTexImage3D );
    SS_LOAD_OPENGL_EXTENSION( glPrimitiveRestartIndex );
    SS_LOAD_OPENGL_EXTENSION( glBlitFramebuffer );
    SS_LOAD_OPENGL_EXTENSION( glBlendFunci );
    SS_LOAD_OPENGL_EXTENSION( glBlendFuncSeparatei );
}

void init_OpenGL_extensions_Win32()
{
    init_required_OpenGL_extensions_Win32();
    init_optional_OpenGL_extensions_Win32();
}

#   endif // SS_OS_WINDOWS
#endif   // End SOLOSNAKE_DEFERRED_RENDERER_USE_GLEW if/else block.

#ifndef SS_DEFERRED_RENDERER_OPENGL_EXTENSIONS
#   error    No OpenGL extensions found for this platform. Please edit this file to correct this.
#endif
