#ifndef SOLOSNAKE_SHADERGL_HPP
#define SOLOSNAKE_SHADERGL_HPP

#include <memory>
#include <string_view>
#include "solosnake/opengl.hpp"

namespace solosnake
{
    /// Class which owns and manages an OpenGL shader program object.
    /// This object is immutable and safe to copy.
    class ShaderGL
    {
    public:

        /// @Throws a solosnake::Error in the event of a problem.
        explicit ShaderGL(
            std::shared_ptr<OpenGL> gl,
            const std::string_view& vertex_shader,
            const std::string_view& fragment_shader);

        ~ShaderGL() noexcept;

        /// Returns the number indicating the location of named uniform in
        /// this shader program, else throws. Zero is a valid return value.
        /// @name Must be a null terminated string that contains no white space.
        int get_uniform_location(const std::string_view& name);

        /// Returns the shader program object value suitable for use with
        /// `glUseProgram`.
        unsigned int get_program() const noexcept;

        /// Returns the stored OpenGL instance.
        OpenGL* get_opengl() const noexcept;

    private:

        void compile_shader(const std::string_view &, unsigned int);

        void link_shader(unsigned int);

    private:
        class Objects;
        std::shared_ptr<Objects> m_objects;
    };

}

#endif
