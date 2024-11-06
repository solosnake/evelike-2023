#ifndef SOLOSNAKE_SKYBOX_HPP
#define SOLOSNAKE_SKYBOX_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include "solosnake/opengl.hpp"
#include "solosnake/shadergl.hpp"

namespace solosnake
{
    class Skybox
    {
    public:

        struct Header
        {
            std::uint8_t  header[4]             { 'S', 'K', 'Y', '1' };
            std::uint16_t side_length           { 0u };
        };

        /// @param skybox_file  The binary file containing 6 pixel buffers, one
        ///                     for each side.
        /// @param shared_ptr   Pointer to shared OpenGL instance.
        explicit Skybox(const char* skybox_file,
                        std::shared_ptr<OpenGL> opengl);

        Skybox(const Skybox&) = delete;

        ~Skybox() noexcept;

        void draw(const float* view4x4) const noexcept;

    private:
        ShaderGL                    m_shader;
        unsigned int                m_cube_map;
        unsigned int                m_skybox_vao;
        int                         m_uniform_view;
    };

    /// Reads the 6 named images from file, checks they meet the requirements
    /// for a skybox, and saves them to the named Skybox file.
    void save_skybox(const std::string_view& filename,
                     const std::array<std::string_view, 6>& images);
}

#endif // SOLOSNAKE_SKYBOX_HPP