#include <cassert>
#include <stdexcept>
#include <string>
#include <string_view>
#include "solosnake/opengl.hpp"
#include "solosnake/shadergl.hpp"

namespace solosnake
{
    /// Class which manages the OpenGL shader objects lifetimes.
    class ShaderGL::Objects
    {
    public:
        explicit Objects(std::shared_ptr<OpenGL>);
        ~Objects() noexcept;


        std::shared_ptr<OpenGL>  m_opengl;
        unsigned int             m_shader_program{0u};
        unsigned int             m_vertex_shader{0u};
        unsigned int             m_fragment_shader{0u};
    };

    ShaderGL::Objects::Objects(std::shared_ptr<OpenGL> gl)
    : m_opengl(gl)
    , m_shader_program{0u}
    , m_vertex_shader{0u}
    , m_fragment_shader{0u}
    {
        assert(gl);
    }

    ShaderGL::Objects::~Objects() noexcept
    {
        m_opengl->DeleteProgram(m_shader_program);
        m_opengl->DeleteShader(m_vertex_shader);
        m_opengl->DeleteShader(m_fragment_shader);
        m_shader_program  = 0u;
        m_vertex_shader   = 0u;
        m_fragment_shader = 0u;
    }

    //--------------------------------------------------------------------------

    ShaderGL::ShaderGL(
        std::shared_ptr<OpenGL> gl,
        const std::string_view &vertex_shader,
        const std::string_view &fragment_shader)
        : m_objects()
    {
        assert(gl);
        auto objects = std::make_shared<ShaderGL::Objects>(gl);

        objects->m_vertex_shader = gl->CreateShader(OpenGL::ShaderType::VERTEX);
        gl->VerifyNoError("glCreateShader(GL_VERTEX_SHADER) failed.");

        objects->m_fragment_shader = gl->CreateShader(OpenGL::ShaderType::FRAGMENT);
        gl->VerifyNoError("glCreateShader(GL_FRAGMENT_SHADER) failed.");

        objects->m_shader_program = gl->CreateProgram();
        gl->VerifyNoError("glCreateProgram failed.");

        gl->CompileShader(vertex_shader.data(),   objects->m_vertex_shader);
        gl->CompileShader(fragment_shader.data(), objects->m_fragment_shader);

        gl->AttachShader(objects->m_shader_program, objects->m_vertex_shader);
        gl->VerifyNoError("glAttachShader (vertex) failed.");

        gl->AttachShader(objects->m_shader_program, objects->m_fragment_shader);
        gl->VerifyNoError("glAttachShader (fragment) failed.");

        gl->LinkShader(objects->m_shader_program);

        m_objects.swap(objects);
    }

    ShaderGL::~ShaderGL() noexcept
    {
    }

    OpenGL* ShaderGL::get_opengl() const noexcept
    {
        return m_objects->m_opengl.get();
    }

    unsigned int ShaderGL::get_program() const noexcept
    {
        return m_objects->m_shader_program;
    }

    int ShaderGL::get_uniform_location(const std::string_view &name)
    {
        auto location = m_objects->m_opengl->GetUniformLocation(m_objects->m_shader_program, name.data());

        if (-1 == location)
        {
            const auto problem = std::string("No uniform '")
                               + std::string(name)
                               + std::string("' in shader.");
            throw std::runtime_error(problem);
        }

        return location;
    }
}