#include <cassert>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <vector>
#include "GL/glew.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "solosnake/opengl.hpp"
#include "solosnake/tga.hpp"

namespace solosnake
{
    namespace
    {

#if 0
        void err(const char* msg, const long line)
        {
            std::cerr << __FILE__ << " " << line << " " << msg << "\n";
        }

        //! Checks OpenGL error state, clearing it, logs any error and the
        //! line number, and return true if the OpenGL error state was not
        //!  GL_NO_ERROR.
        static bool opengl_no_errors(const long line)
        {
            auto e = glGetError();

            switch (e)
            {
            case GL_NO_ERROR:
                return true;
            case GL_INVALID_ENUM:
                err("GL_INVALID_ENUM detected at line ", line);
                break;
            case GL_INVALID_VALUE:
                err("GL_INVALID_VALUE detected at line ", line);
                break;
            case GL_INVALID_OPERATION:
                err("GL_INVALID_OPERATION detected at line ", line);
                break;
            case GL_STACK_OVERFLOW:
                err("GL_STACK_OVERFLOW detected at line ", line);
                break;
            case GL_STACK_UNDERFLOW:
                err("GL_STACK_UNDERFLOW detected at line ", line);
                break;
            case GL_OUT_OF_MEMORY:
                err("GL_OUT_OF_MEMORY detected at line ", line);
                break;
            default:
                err("Unknown OpenGL error code detected at line ", line);
                break;
            }

            assert(!"OpenGL error encountered");
            return false;
        }
#endif


        /// Throws a runtime_error if `glGetError` is not `GL_NO_ERROR`.
        void gl_error_check(const char* msg)
        {
            if (GL_NO_ERROR != glGetError())
            {
                throw std::runtime_error(msg);
            }
        }

        void GLAPIENTRY debug_callback(GLenum source,
                                       GLenum type,
                                       GLuint ,
                                       GLenum severity,
                                       GLsizei ,
                                       const GLchar* message,
                                       const void*)
        {
            auto src = "Unknown Source";
            auto cat = "UNKNOWN TYPE";
            auto lvl = "Unknown Severity";
            auto msg = message ? message : "Unknown Message";

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:
                src = "API";
                break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                src = "WINDOW SYSTEM";
                break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                src = "SHADER COMPILER";
                break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
                src = "THIRD PARTY";
                break;

            case GL_DEBUG_SOURCE_APPLICATION:
                src = "APPLICATION";
                break;

            case GL_DEBUG_SOURCE_OTHER:
                src = "OTHER";
                break;

            default:
                break;
            }

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:
                cat = "ERROR";
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                cat = "DEPRECATED";
                break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                cat = "UNDEFINED BEHAVIOR";
                break;

            case GL_DEBUG_TYPE_PORTABILITY:
                cat = "PORTABILITY";
                break;

            case GL_DEBUG_TYPE_PERFORMANCE:
                cat = "PERFORMANCE";
                break;

            case GL_DEBUG_TYPE_MARKER:
                cat = "MARKER";
                break;

            case GL_DEBUG_TYPE_PUSH_GROUP:
                cat = "PUSH GROUP";
                break;

            case GL_DEBUG_TYPE_POP_GROUP:
                cat = "POP GROUP";
                break;

            case GL_DEBUG_TYPE_OTHER:
                cat = "OTHER";
                break;

            default:
                break;
            };

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_LOW:
                lvl = "LOW";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                lvl = "MEDIUM";
                break;

            case GL_DEBUG_SEVERITY_HIGH:
                lvl = "HIGH";
                break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
                lvl = "NOTE";
                break;

            default:
                break;
            }

            std::cerr << "OpenGL " << ": '" << msg << "' (" << src
                      << ", " << lvl << ", " << cat << ")\n";
        }
    }

    OpenGL::OpenGL()
        : m_program_object{0u}
        , m_vertex_array_object{0u}
        , m_enabled_flags{0u}
        , m_blend_src_rgb{GL_ONE}
        , m_blend_src_alpha{GL_ONE}
        , m_blend_dst_rgb{GL_ZERO}
        , m_blend_dst_alpha{GL_ZERO}
        , m_depth_func{GL_LESS}
        , m_clear_color{0.0f, 0.0f, 0.0f, 0.0f}
        , m_depth_testing{false}
        , m_depth_clamping{false}
        , m_depth_writing{false}
        , m_back_face_culling_enabled{false}
        , m_wireframe{false}
        , m_point_size{false}
        , m_blending{false}
    {
        // Initialize OpenGL Extension Wrangler
        ::glewExperimental = GL_TRUE;
        auto glewError = glewInit();
        if (GLEW_OK != glewError)
        {
            auto problem = glewGetErrorString(glewError);
            throw std::runtime_error(std::string("glewGetErrorString failed: ")
                + reinterpret_cast<const char*>(problem));
        }

        VerifyNoError("Pre-existing OpenGL error.");

        // What is currently active program
        GLint program{0};
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        gl_error_check("glGetIntegerv GL_CURRENT_PROGRAM failed.");
        m_program_object = static_cast<GLuint>(program);

        // Get currently bound VAO (if any).
        GLint vao{0};
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
        m_vertex_array_object = static_cast<GLuint>(vao);

        // Get current clear color.
        glGetFloatv(GL_COLOR_CLEAR_VALUE, m_clear_color);

        // Is wireframe enabled?
        GLint polygon_mode[2] = { 0, 0 };
        glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
        m_wireframe = (GL_LINE == polygon_mode[0]);

        // What is the depth function?
        GLint depth_func{GL_LESS};
        glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
        m_depth_func = depth_func;

        // Is depth testing enabled?
        m_depth_testing = glIsEnabled(GL_DEPTH_TEST);

        // Is depth clamping enabled? The clipping behavior against the Z
        // position of a vertex can be TURNED OFF by activating depth clamping.
        m_depth_clamping = glIsEnabled(GL_DEPTH_CLAMP);

        // Is writing to depth buffer enabled?
        GLint depth_mask{0};
        glGetIntegerv(GL_DEPTH_WRITEMASK, &depth_mask);
        m_depth_writing = depth_mask == GL_TRUE;

        // Is face culling enabled?
        m_back_face_culling_enabled = glIsEnabled(GL_CULL_FACE);

        // Is program point-size enabled?
        m_point_size = glIsEnabled(GL_PROGRAM_POINT_SIZE);

        // Is blending enabled?
        m_blending = glIsEnabled(GL_BLEND);

        // Get blending settings.
        glGetIntegerv(GL_BLEND_SRC_RGB,   &m_blend_src_rgb);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_blend_src_alpha);
        glGetIntegerv(GL_BLEND_DST_RGB,   &m_blend_dst_rgb);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &m_blend_dst_alpha);

        VerifyNoError("Failed to construct 'OpenGL' class.");
    }

    void OpenGL::ActiveTexture(TextureTarget target) noexcept
    {
        glActiveTexture( static_cast<unsigned int>(target) );
    }

    void OpenGL::AttachShader( const unsigned int program,
                               const unsigned int shader ) noexcept
    {
        glAttachShader( program, shader );
    }

    void OpenGL::BindBuffer(const Target target,
                            const unsigned int buffer) noexcept
    {
        glBindBuffer( static_cast<unsigned int>(target), buffer );
    }

    void OpenGL::BindTexture(const TextureType type,
                             const unsigned int texture) noexcept
    {
        glBindTexture(static_cast<unsigned int>(type), texture);
    }

    void OpenGL::BindVertexArray(const unsigned int vao) noexcept
    {
        if (vao != m_vertex_array_object)
        {
            m_vertex_array_object = vao;
            glBindVertexArray(vao);
        }
    }

    void OpenGL::BlendFunc(const Blending src, const Blending dst) noexcept
    {
        // Presently we do not support separable blending.
        assert( m_blend_src_rgb == m_blend_src_alpha );
        assert( m_blend_dst_rgb == m_blend_dst_alpha );

        if(static_cast<int>(src) != m_blend_src_rgb or
           static_cast<int>(dst) != m_blend_dst_rgb)
        {
            m_blend_src_rgb   = static_cast<int>(src);
            m_blend_src_alpha = static_cast<int>(src);
            m_blend_dst_rgb   = static_cast<int>(dst);
            m_blend_dst_alpha = static_cast<int>(dst);
            glBlendFunc( static_cast<GLenum>(src), static_cast<GLenum>(dst) );
        }
    }

    void OpenGL::BufferData(const Target target, const std::ptrdiff_t size,
                            const void * data, const Usage usage) noexcept
    {
        glBufferData( static_cast<unsigned int>(target), size, data,
                      static_cast<unsigned int>(usage) );
    }

    unsigned int OpenGL::CreateProgram() noexcept
    {
        return glCreateProgram();
    }

    unsigned int OpenGL::CreateShader( ShaderType type ) noexcept
    {
        return glCreateShader( static_cast<unsigned int>(type) );
    }

    void OpenGL::CompileShader( const unsigned int shader ) noexcept
    {
        glCompileShader( shader );
    }

    void OpenGL::CompileShader(const char* source, const unsigned int shader)
    {
        assert( source );

        VerifyNoError("Existing GL error.");

        ShaderSource(shader, 1, &source, nullptr);
        VerifyNoError("glShaderSource failed.");

        CompileShader(shader);
        VerifyNoError("glCompileShader failed.");

        int status{0};
        GetShaderiv(shader, ShaderParam::COMPILE_STATUS, &status);
        VerifyNoError("glGetShaderiv GL_COMPILE_STATUS failed.");

        if (0 == status)
        {
            // Something went wrong. Examine log and throw.
            int log_size{0};
            GetShaderiv(shader, ShaderParam::INFO_LOG_LENGTH, &log_size);
            VerifyNoError("glGetShaderiv GL_INFO_LOG_LENGTH failed.");

            std::string log_message(static_cast<std::size_t>(log_size + 2), char{0});
            GetShaderInfoLog(shader, log_size + 1, nullptr, log_message.data());
            VerifyNoError("glGetShaderInfoLog failed.");

            throw std::runtime_error(log_message);
        }
    }

    void OpenGL::Clear( const unsigned int flags ) noexcept
    {
        glClear( flags );
    }

    void OpenGL::ClearColor(const float r,
                            const float g,
                            const float b,
                            const float a) noexcept
    {
        if (r != m_clear_color[0] || g != m_clear_color[1] || b != m_clear_color[2] || a != m_clear_color[3])
        {
            m_clear_color[0] = r;
            m_clear_color[1] = g;
            m_clear_color[2] = b;
            m_clear_color[3] = a;
            glClearColor(r, g, b, a);
        }
    }

    void OpenGL::DeleteBuffers(const int n,
                               const unsigned int* buffers) noexcept
    {
        glDeleteBuffers(n, buffers);
    }

    void OpenGL::DeleteProgram(const unsigned int program) noexcept
    {
        glDeleteProgram( program );
    }

    void OpenGL::DeleteShader(const unsigned int shader) noexcept
    {
        glDeleteShader( shader );
    }

    void OpenGL::DeleteTexture(unsigned int* texture) noexcept
    {
        if( texture )
        {
            glDeleteTextures( 1, texture );
            *texture = 0u;
        }
    }

    void OpenGL::DeleteTextures(const int n,
                                const unsigned int* textures) noexcept
    {
        glDeleteTextures( n, textures );
    }

    void OpenGL::DeleteVertexArrays(const int n,
                                    const unsigned int* arrays) noexcept
    {
        glDeleteVertexArrays(n, arrays);
    }

    void OpenGL::DepthFunc( const DepthAlgo algo )
    {
        if( static_cast<DepthAlgo>(m_depth_func) != algo )
        {
            m_depth_func = static_cast<int>(algo);
            glDepthFunc( static_cast<GLenum>(algo) );
        }
    }

    void OpenGL::DisableDebugOutput() noexcept
    {
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                             GL_DEBUG_TYPE_OTHER,
                             0,
                             GL_DEBUG_SEVERITY_NOTIFICATION,
                             -1,
                             "Debug callbacks OFF.");

#ifndef NDEBUG
        // Debug build: synchronise OpenGL error detection with the OpenGL
        // callback.
        glDisable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
#endif
        glDisable( GL_DEBUG_OUTPUT );

        glDebugMessageCallback(nullptr, nullptr);
    }


    void OpenGL::DrawArrays(OpenGL::Mode mode,
                            const int first,
                            const unsigned int count) noexcept
    {
        glDrawArrays( static_cast<unsigned int>(mode), first, count );
    }

    void OpenGL::DrawElements(const Mode mode,
                              const unsigned int count,
                              const IndexType type,
                              const void * indices) noexcept
    {
        glDrawElements( static_cast<unsigned int>(mode), count,
                        static_cast<unsigned int>(type), indices );
    }

    void OpenGL::DrawIndexedPoints( const unsigned int point_count,
                                    const std::uint16_t* indices ) noexcept
    {
        glDrawElements( GL_POINTS, point_count, GL_UNSIGNED_SHORT, indices );
    }

    void OpenGL::DrawPointArrays( unsigned int point_count ) noexcept
    {
        glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>(point_count) );
    }

    /// Disabled by default in OpenGL. Enables culling of back faces.
    /// There are three settings in OpenGL related to this: `glCullFace`
    /// which sets which face(s) to cull, `GL_CULL_FACE` which enables or
    /// disables culling, and `glFrontFace` which determines what a
    /// front-facing polygon is (clockwise or counter-clockwise).
    /// This call sets the front face to counter-clockwise (OpenGL's default)
    /// and sets back faces to be culler, and toggles culling on or off.
    void OpenGL::EnableBackFaceCulling(const bool enabled) noexcept
    {
        if (enabled != m_back_face_culling_enabled)
        {
            m_back_face_culling_enabled = enabled;
            if (enabled)
            {
                glEnable(GL_CULL_FACE);
                glFrontFace(GL_CCW);
                glCullFace(GL_BACK);
            }
            else
            {
                glDisable(GL_CULL_FACE);
                glFrontFace(GL_CCW);
                glCullFace(GL_BACK);
            }
        }
    }

    void OpenGL::EnableBlending(const bool enabled) noexcept
    {
        if (enabled != m_blending)
        {
            m_blending = enabled;
            if (enabled)
            {
                glEnable(GL_BLEND);
            }
            else
            {
                glDisable(GL_BLEND);
            }
        }
    }

    void OpenGL::EnableDepthTest(const bool enabled) noexcept
    {
        if (enabled != m_depth_testing)
        {
            m_depth_testing = enabled;
            if (enabled)
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
        }
    }

    void OpenGL::EnableDepthClamp(const bool enabled) noexcept
    {
        if (enabled != m_depth_clamping)
        {
            m_depth_clamping = enabled;
            if (enabled)
            {
                glEnable(GL_DEPTH_CLAMP);
            }
            else
            {
                glDisable(GL_DEPTH_CLAMP);
            }
        }
    }

    void OpenGL::EnableDepthWrite(const bool enabled) noexcept
    {
        if (enabled != m_depth_writing)
        {
            m_depth_writing = enabled;
            if (enabled)
            {
                glDepthMask(GL_TRUE);
            }
            else
            {
                glDepthMask(GL_FALSE);
            }
        }
    }

    void OpenGL::EnableLinePolygonMode(const bool enabled) noexcept
    {
        if (enabled != m_wireframe)
        {
            m_wireframe = enabled;
            if (enabled)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    }

    void OpenGL::EnableProgramPointSize(const bool enabled) noexcept
    {
        if (enabled != m_point_size)
        {
            m_point_size = enabled;
            if (enabled)
            {
                glEnable(GL_PROGRAM_POINT_SIZE);
            }
            else
            {
                glDisable(GL_PROGRAM_POINT_SIZE);
            }
        }
    }

    void OpenGL::EnableDebugOutput() noexcept
    {
        // Enable OpenGL debugging and logging.
        glDebugMessageCallback(debug_callback, nullptr);

        glEnable( GL_DEBUG_OUTPUT );

#ifndef NDEBUG
        // Debug build: synchronise OpenGL error detection with the OpenGL
        // callback.
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
#endif
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                             GL_DEBUG_TYPE_OTHER,
                             0,
                             GL_DEBUG_SEVERITY_NOTIFICATION,
                             -1,
                             "Debug callbacks ON.");
    }

    void OpenGL::EnableVertexAttribArray(unsigned int index) noexcept
    {
        glEnableVertexAttribArray(index);
    }

    /// GL_NO_ERROR is 0.
    int OpenGL::GetError() noexcept
    {
        return glGetError();
    }

    void OpenGL::GenBuffers(const int n, unsigned int* buffers) noexcept
    {
        glGenBuffers(n, buffers);
    }

    void OpenGL::GenVertexArrays(const int n, unsigned int* arrays) noexcept
    {
        glGenVertexArrays(n, arrays);
    }

    int OpenGL::GetUniformLocation(const unsigned int program,
                                   const char* name) noexcept
    {
        return glGetUniformLocation( program, name );
    }

    OpenGL::Viewport OpenGL::GetViewport() noexcept
    {
        GLint viewport[4] = {0};
        glGetIntegerv(GL_VIEWPORT, viewport);
        return Viewport{ viewport[0], viewport[1], viewport[2], viewport[3] };
    }

    void OpenGL::GetShaderiv(const unsigned int shader,
                             const ShaderParam pname,
                             int* params) noexcept
    {
        glGetShaderiv( shader, static_cast<unsigned int>(pname), params );
    }

    void OpenGL::GetShaderInfoLog(const unsigned int shader,
                                  const int maxLength,
                                  int* length,
                                  char* infoLog) noexcept
    {
        glGetShaderInfoLog( shader, maxLength, length, infoLog );
    }

    void OpenGL::GetProgramiv(const unsigned int program,
                              const ProgramParam pname,
                              int* params) noexcept
    {
        glGetProgramiv( program, static_cast<unsigned int>(pname), params );
    }

    void OpenGL::LinkShader(const unsigned int shader)
    {
        LinkProgram(shader);
        VerifyNoError("glLinkProgram failed.");

        int status{0};
        GetProgramiv(shader, ProgramParam::LINK_STATUS, &status);
        VerifyNoError("glGetShaderiv GL_LINK_STATUS failed.");

        if (0 == status)
        {
            // Something went wrong. Examine log and throw.
            int log_size{0};
            GetShaderiv(shader, ShaderParam::INFO_LOG_LENGTH, &log_size);
            VerifyNoError("glGetShaderiv GL_INFO_LOG_LENGTH failed.");

            std::string log_message(static_cast<std::size_t>(log_size + 2), char{0});
            GetShaderInfoLog(shader, log_size + 1, nullptr, log_message.data());
            VerifyNoError("glGetShaderInfoLog (linking) failed.");

            throw std::runtime_error(log_message);
        }
    }

    void OpenGL::LinkProgram(const unsigned int program) noexcept
    {
        glLinkProgram( program );
    }

    unsigned int OpenGL::MakeCubeMapTexture(int side_length,
                                            std::function<const std::uint8_t*()> callback)
    {
        if( GL_NO_ERROR != glGetError() )
        {
            throw std::runtime_error( "Existing OpenGL error before MakeCubeMapTexture." );
        }

        assert( side_length > 0 );

        GLuint cubeTex = 0;
        glGenTextures( 1, &cubeTex );

        glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

        glBindTexture( GL_TEXTURE_CUBE_MAP, cubeTex );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

        for(std::int32_t i=0; i < 6; ++i)
        {
            auto bgr = callback();
            assert(bgr);

            glTexImage2D( static_cast<GLenum>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i ),
                          0,
                          GL_RGB8,
                          static_cast<GLsizei>( side_length ),
                          static_cast<GLsizei>( side_length ),
                          0,
                          GL_BGR,
                          GL_UNSIGNED_BYTE,
                          bgr );
        }

        // Unbind texture.
        glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

        if( GL_NO_ERROR != glGetError() )
        {
            glDeleteTextures( 1, &cubeTex );
            throw std::runtime_error( "MakeCubeMapTexture failed");
        }

        return static_cast<unsigned int>(cubeTex);
    }

    unsigned int OpenGL::MakeTexture(const void* bgr_texels,
                                     const int width,
                                     const int height,
                                     const bool texels_have_alpha_channel,
                                     const bool make_mip_maps)
    {
        if( GL_NO_ERROR != glGetError() )
        {
            throw std::runtime_error( "Existing OpenGL error before MakeTexture." );
        }

        // Select 3 or 4 byte destination format.
        const auto destination_format = texels_have_alpha_channel ? GL_RGBA8 : GL_RGB8;

        // Select BGRA or BGR source format.
        const auto src_format          = texels_have_alpha_channel ? GL_BGRA  : GL_BGR;

        GLuint texture_id{0u};
        glGenTextures( 1, &texture_id );

        if( 0 == texture_id )
        {
            throw std::runtime_error( "Failed to gen texture." );
        }

        glBindTexture( GL_TEXTURE_2D, texture_id );

        // Check to see we have space...
        glTexImage2D( GL_PROXY_TEXTURE_2D, 0, destination_format, width, height, 0, src_format, GL_UNSIGNED_BYTE, NULL );

        GLint proxy_width = 0;
        glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &proxy_width );

        if( proxy_width > 0 )
        {
            assert( GL_NO_ERROR == glGetError() );

            glTexImage2D( GL_TEXTURE_2D, 0, destination_format, width, height, 0, src_format, GL_UNSIGNED_BYTE, bgr_texels );

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            if( make_mip_maps )
            {
                glGenerateMipmap( GL_TEXTURE_2D );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
            }
            else
            {
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            }

            glBindTexture( GL_TEXTURE_2D, 0 );
        }
        else
        {
            glBindTexture( GL_TEXTURE_2D, 0 );
            DeleteTexture( &texture_id );
            throw std::runtime_error( "Proxy texture check failed." );
        }

        if( GL_NO_ERROR != glGetError() )
        {
            DeleteTexture( &texture_id );
            throw std::runtime_error( "OpenGL error when creating texture." );
        }

        return texture_id;
    }

    void OpenGL::SaveTGAScreenshot(const char* filename)
    {
        GLint viewport[4] = {0};
        glGetIntegerv(GL_VIEWPORT, viewport);

        gl_error_check("Unable to get viewport from OpenGL.");

        auto row_size = ((viewport[2] * 3 + 3) & ~3);
        auto data_size = row_size * viewport[3];
        std::vector<std::uint8_t> buffer(data_size, 0u);

        glReadPixels(0, 0,
                     viewport[2], viewport[3],
                     GL_BGR,
                     GL_UNSIGNED_BYTE,
                     buffer.data());

        gl_error_check("glReadPixels failed.");

        // Flip the TGA because the buffer is already upside down.

        save_bgr_tga( static_cast<std::int16_t>(viewport[2]),
                      static_cast<std::int16_t>(viewport[3]),
                      buffer.data(),
                      true,
                      filename );
    }

    void OpenGL::SetViewport(const int x, const int y,
                             const int width, const int height) noexcept
    {
        glViewport(x, y, width, height);
    }

    void OpenGL::ShaderSource(const unsigned int shader, const int count,
                              const char **texts, const int *length) noexcept
    {
        glShaderSource(shader, count, texts, length );
    }

    void OpenGL::ToggleLinePolygonMode() noexcept
    {
        EnableLinePolygonMode(not m_wireframe);
    }

    void OpenGL::Uniform1i(const int location, const int v0) noexcept
    {
        glUniform1i( location, v0 );
    }

    void OpenGL::Uniform4fv(const int location, const int count, const float* value) noexcept
    {
        glUniform4fv( location, count, value );
    }

    void OpenGL::Uniform1f(const int location, const float value) noexcept
    {
        glUniform1f( location, value );
    }

    void OpenGL::UniformMatrix4fv(const int location, const int count,
                                  const bool transpose, const float* value) noexcept
    {
        glUniformMatrix4fv( location, count, transpose, value );
    }

    void OpenGL::UseProgram(const unsigned int program_object) noexcept
    {
        if (program_object != m_program_object)
        {
            m_program_object = program_object;
            glUseProgram(program_object);
        }
    }

    void OpenGL::VerifyNoError(const char* msg)
    {
        if (GL_NO_ERROR != glGetError())
        {
            throw std::runtime_error(msg);
        }
    }

    void OpenGL::VertexAttribPointer(const unsigned int index,
                                     const int size,
                                     const DataType type,
                                     const bool normalized,
                                     const int stride,
                                     const void * pointer) noexcept
    {
        glVertexAttribPointer(index, size, static_cast<unsigned int>(type),
                              normalized, stride, pointer );
    }
}