#include <cassert>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <vector>
#include "solosnake/skybox.hpp"
#include "solosnake/image.hpp"

namespace solosnake
{
    namespace
    {
        /// Resizes the buffer and reads into it n bytes from f.
        void read_bytes(std::ifstream& f, std::size_t n, std::vector<std::uint8_t>& buffer)
        {
            buffer.resize(n, char(0));
            f.read(reinterpret_cast<char*>(buffer.data()), n);
        }

        static constexpr auto vert_shader =
            "#version 420 core                                      \n"
            "out VS_OUT { vec3 tc; } vs_out;                        \n"
            "uniform mat4 view_matrix;                              \n"
            "void main(void) {                                      \n"
            "vec3[4] vertices = vec3[4](vec3(-1.0, -1.0, 1.0),      \n"
            "                           vec3( 1.0, -1.0, 1.0),      \n"
            "                           vec3(-1.0,  1.0, 1.0),      \n"
            "                           vec3( 1.0,  1.0, 1.0));     \n"
            "vs_out.tc = mat3(view_matrix) * vertices[gl_VertexID]; \n"
            "gl_Position = vec4(vertices[gl_VertexID], 1.0);        \n"
            "}"
            ;

        static constexpr auto frag_shader =
            "#version 420 core                                      \n"
            "layout(binding = 0) uniform samplerCube tex_cubemap;   \n"
            "layout(location = 0) out vec4 color;                   \n"
//            "layout(depth_greater) out float gl_FragDepth;          \n"
            "in VS_OUT { vec3 tc; } fs_in;                          \n"
            "void main(void) {                                      \n"
            "   color = texture(tex_cubemap, fs_in.tc);             \n"
//            "   gl_FragDepth = 1.0;                                 \n"
            "}"
            ;
    }

    Skybox::Skybox(const char* cube_images,
                   std::shared_ptr<OpenGL> gl)
    : m_shader(gl, vert_shader, frag_shader)
    , m_cube_map(0u)
    , m_skybox_vao(0u)
    , m_uniform_view(0)
    {
        std::ifstream cube_images_file;
        cube_images_file.open(cube_images, std::ios::binary | std::ios::in);

        if (cube_images_file.is_open())
        {
            std::vector<std::uint8_t> buffer;

            // Read header.
            read_bytes(cube_images_file, sizeof(Skybox::Header), buffer);
            auto header = *reinterpret_cast<const Skybox::Header*>(buffer.data());

            if(header.header[0] != 'S' or
               header.header[1] != 'K' or
               header.header[2] != 'Y' or
               header.header[3] != '1')
            {
                std::stringstream ss;
                ss << "File '" << cube_images << "' does not appear to be a Skybox file.";
                throw std::runtime_error(ss.str());
            }

            const auto side_length = header.side_length;
            const auto pixel_buffer_size = 3 * sizeof(std::uint8_t) * side_length * side_length;
            buffer.reserve( pixel_buffer_size );

            // Each time this is called a new pixel_buffer_size chunk is read
            // from the file into the buffer.
            auto callback = [&buffer, &cube_images_file, pixel_buffer_size]() -> const std::uint8_t*
            {
                read_bytes(cube_images_file, pixel_buffer_size, buffer);
                return buffer.data();
            };

            try
            {
                m_cube_map = gl->MakeCubeMapTexture(side_length, callback);
                gl->GenVertexArrays(1, &m_skybox_vao);
                m_uniform_view = m_shader.get_uniform_location("view_matrix");
                cube_images_file.close();
            }
            catch(const std::exception& e)
            {
                gl->DeleteTexture(&m_cube_map);
                gl->DeleteVertexArrays(1, &m_skybox_vao);
                throw e;
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to open Skybox '" << cube_images << "' for reading.";
            throw std::runtime_error(ss.str());
        }
    }

    Skybox::~Skybox() noexcept
    {
        auto gl = m_shader.get_opengl();

        if(gl)
        {
            gl->DeleteTexture(&m_cube_map);
            gl->DeleteVertexArrays(1, &m_skybox_vao);
        }
    }

    void Skybox::draw(const float* view4x4) const noexcept
    {
        assert(view4x4);
        auto gl = m_shader.get_opengl();
        gl->UseProgram(m_shader.get_program());
        gl->BindTexture(OpenGL::TextureType::TEXTURE_CUBE_MAP, m_cube_map);
        gl->BindVertexArray(m_skybox_vao);
        gl->UniformMatrix4fv(m_uniform_view, 1, false, view4x4);
        gl->DrawArrays(OpenGL::Mode::TRIANGLE_STRIP, 0, 4);
    }

    void save_skybox(const std::string_view& filename,
                     const std::array<std::string_view, 6>& images)
    {
        if(not filename.empty())
        {
            try
            {
                std::stringstream ss;

                // Open file to write to.
                std::ofstream sky_file(filename.data(), std::ios::binary | std::ios::out);

                if (sky_file.is_open())
                {
                    // Throw when problems are encountered.
                    sky_file.exceptions(std::ifstream::failbit|std::ifstream::badbit);

                    unsigned int side_length = 0;

                    // Open each image in turn. Can be whatever type Image
                    // supports e.g BMP or PNG etc.
                    for(int i=0; i < 6; ++i)
                    {
                        Image img( images[i], Image::Format::BGR );

                        if(img.width() != img.height())
                        {
                            ss << images[i] << " not square.";
                            throw std::runtime_error(ss.str());
                        }

                        if(img.height() > std::numeric_limits<std::uint16_t>::max())
                        {
                            ss << images[i] << " too large.";
                            throw std::runtime_error(ss.str());
                        }

                        if(i == 0)
                        {
                            // Capture side-length of first image.
                            side_length = img.height();

                            // Write header.
                            Skybox::Header header;
                            header.side_length = static_cast<std::uint16_t>(side_length);
                            sky_file.write( reinterpret_cast<const char*>(&header), sizeof(header) );
                        }
                        else
                        {
                            // Check it's size matches the earlier images.
                            if(img.height() !=  side_length)
                            {
                                ss << images[i] << " different size.";
                                throw std::runtime_error(ss.str());
                            }
                        }

                        sky_file.write( reinterpret_cast<const char*>(img.pixels()), img.bytes_size() );
                    }

                    sky_file.close();
                }
            }
            catch(const std::exception& e)
            {
                std::remove(filename.data());
                throw;
            }
        }
    }
}