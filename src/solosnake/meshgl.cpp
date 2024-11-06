#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include "solosnake/meshgl.hpp"
#include "solosnake/image.hpp"

namespace solosnake
{
    namespace
    {
        Image make_default_texture_image(const Image::Format format)
        {
            constexpr auto w = 256u;
            constexpr auto h = 256u;

            Image img(w, h, format);

            const Bgra yellow{ 0, 255, 255, 255 };
            const Bgra purple{ 128, 0, 128, 255 };

            img.clear_to_bgra( yellow );

            for(auto y=0u; y < h; y += 4u)
            {
                for(auto x=0u; x < w; x += 4u)
                {
                    img.set_pixel(     x,      y, purple);
                    img.set_pixel(     x, 1u + y, purple);
                    img.set_pixel(1u + x,      y, purple);
                    img.set_pixel(1u + x, 1u + y, purple);
                }
            }

            return img;
        }

        /// Resizes the buffer and reads into it n bytes from f.
        void read_bytes(std::ifstream& f, std::size_t n, std::vector<std::uint8_t>& buffer)
        {
            buffer.resize(n, char(0));
            f.read(reinterpret_cast<char*>(buffer.data()), n);
        }
    }

    //--------------------------------------------------------------------------

    /// Class which manages the OpenGL mesh objects lifetimes.
    class MeshGL::Objects
    {
    public:
        explicit Objects(std::shared_ptr<OpenGL>);
        ~Objects() noexcept;

        void load_mesh(const MeshHeader&, const std::uint8_t*);
        void load_base_color_rgb(const MeshHeader&, const std::uint8_t*);
        void load_orm_rgb(const MeshHeader&, const std::uint8_t*);
        void load_normal_rgb(const MeshHeader&, const std::uint8_t*);
        void load_emissive_team_rgba(const MeshHeader&, const std::uint8_t*);

        std::shared_ptr<OpenGL> m_opengl;
        std::array<float,3u>    m_centre_xyz {0.0f, 0.0f, 0.0f};
        float                   m_bounding_radius{0.0f};
        int                     m_count{0u};
        unsigned int            m_base_texture{0u};
        unsigned int            m_normals_texture{ 0u };
        unsigned int            m_orm_texture{ 0u };
        unsigned int            m_emissive_team_texture{ 0u };
        unsigned int            m_vertex_array_object{0u};
        unsigned int            m_buffer_objects[5]{0u, 0u, 0u, 0u, 0u};
        OpenGL::IndexType       m_index_type{OpenGL::IndexType::UNSIGNED_SHORT};
    };

    MeshGL::Objects::Objects(std::shared_ptr<OpenGL> gl)
    : m_opengl(gl)
    {
        assert( gl );
    }

    void MeshGL::Objects::load_base_color_rgb(const MeshHeader& header, const std::uint8_t* pixels)
    {
        constexpr auto have_alpha   = false;
        constexpr auto make_mipmaps = true;
        if(header.base_color_size[0] > 0u and
           header.base_color_size[1] > 0u and
           pixels != nullptr)
        {
            assert(pixels);
            m_base_texture = m_opengl->MakeTexture(pixels,
                                                   header.base_color_size[0],
                                                   header.base_color_size[1],
                                                   have_alpha,
                                                   make_mipmaps);
        }
        else
        {
            // No base color texture. Make a default one.
            auto base = make_default_texture_image(Image::Format::BGR);
            m_base_texture = m_opengl->MakeTexture(base.data(),
                                                   base.width(),
                                                   base.height(),
                                                   have_alpha,
                                                   make_mipmaps);
        }
    }

    void MeshGL::Objects::load_normal_rgb(const MeshHeader& header, const std::uint8_t* pixels)
    {
        constexpr auto have_alpha   = false;
        constexpr auto make_mipmaps = true;
        if (header.normal_size[0] > 0u and header.normal_size[1] > 0u)
        {
            assert(pixels);
            m_normals_texture = m_opengl->MakeTexture(pixels,
                                                     header.normal_size[0],
                                                     header.normal_size[1],
                                                     have_alpha,
                                                     make_mipmaps);
        }
        else
        {
            // Make default normals texture (the purple/blue looking one).
            Image normals(256u, 256u, Bgr{255, 128, 128});
            m_normals_texture = m_opengl->MakeTexture(normals.data(),
                                                      normals.width(),
                                                      normals.height(),
                                                      have_alpha,
                                                      make_mipmaps);
        }
    }

    void MeshGL::Objects::load_orm_rgb(const MeshHeader& header, const std::uint8_t* pixels)
    {
        constexpr auto have_alpha   = false;
        constexpr auto make_mipmaps = true;
        if (header.orm_size[0] > 0u and header.orm_size[1] > 0u)
        {
            assert(pixels);
            m_orm_texture = m_opengl->MakeTexture(pixels,
                                                  header.orm_size[0],
                                                  header.orm_size[1],
                                                  have_alpha,
                                                  make_mipmaps);
        }
        else
        {
            // Make default occlusion/roughness/metallic texture (white).
            Image orm(256u, 256u, Bgr{ 255, 255, 255 });
            m_orm_texture = m_opengl->MakeTexture(orm.data(),
                                                  orm.width(),
                                                  orm.height(),
                                                  have_alpha,
                                                  make_mipmaps);
        }
    }


    void MeshGL::Objects::load_emissive_team_rgba(const MeshHeader& header, const std::uint8_t* pixels)
    {
        constexpr auto have_alpha   = true;
        constexpr auto make_mipmaps = true;
        if (header.emissive_team_size[0] > 0u and header.emissive_team_size[1] > 0u)
        {
            assert(pixels);
            m_emissive_team_texture = m_opengl->MakeTexture(pixels,
                                                            header.emissive_team_size[0],
                                                            header.emissive_team_size[1],
                                                            have_alpha,
                                                            make_mipmaps);
        }
        else
        {
            // Make default emissive/team texture (white).
            Image orm(256u, 256u, Bgra{ 255, 255, 255, 255 });
            m_emissive_team_texture = m_opengl->MakeTexture(orm.data(),
                                                            orm.width(),
                                                            orm.height(),
                                                            have_alpha,
                                                            make_mipmaps);
        }
    }

    void MeshGL::Objects::load_mesh(const MeshHeader& header, const std::uint8_t* data)
    {
        constexpr unsigned int pos_location      = 0u;
        constexpr unsigned int normals_location  = 1u;
        constexpr unsigned int tangents_location = 2u;
        constexpr unsigned int uvs_location = 3u;

        if (0u != (header.index_count % 3u))
        {
            throw std::runtime_error("Mesh indices not a multiple of 3.");
        }

        m_count           = static_cast<int>(header.index_count);
        m_bounding_radius = header.bounding_radius;
        m_centre_xyz      = { header.centre[0], header.centre[1], header.centre[2] };
        m_index_type      = header.index_size == sizeof(std::uint16_t)
                            ? OpenGL::IndexType::UNSIGNED_SHORT
                            : OpenGL::IndexType::UNSIGNED_INT;

        auto index_bytes = header.index_size * header.index_count;
        auto vec2s_bytes = sizeof(float) * 2u * header.vertex_count;
        auto vec3s_bytes = sizeof(float) * 3u * header.vertex_count;
        auto vec4s_bytes = sizeof(float) * 4u * header.vertex_count;

        m_opengl->GenVertexArrays(1, &m_vertex_array_object);
        m_opengl->VerifyNoError("glGenVertexArrays failed.");

        m_opengl->GenBuffers(5u, m_buffer_objects);
        m_opengl->VerifyNoError("glGenBuffers failed.");

        m_opengl->BindVertexArray(m_vertex_array_object);
        m_opengl->VerifyNoError("glBindVertexArray failed.");

        // Upload triangle indices (unsigned short)
        const std::uint8_t* upload = data;
        m_opengl->BindBuffer(OpenGL::Target::ELEMENT_ARRAY_BUFFER, m_buffer_objects[0]);
        m_opengl->BufferData(OpenGL::Target::ELEMENT_ARRAY_BUFFER, index_bytes, upload, OpenGL::Usage::STATIC_DRAW);
        m_opengl->VerifyNoError("Upload triangle indices failed.");

        // Upload vertices (VEC3)
        upload += index_bytes;
        m_opengl->BindBuffer(OpenGL::Target::ARRAY_BUFFER, m_buffer_objects[1]);
        m_opengl->BufferData(OpenGL::Target::ARRAY_BUFFER, vec3s_bytes, upload, OpenGL::Usage::STATIC_DRAW);
        m_opengl->EnableVertexAttribArray(pos_location);
        m_opengl->VertexAttribPointer(pos_location, 3, OpenGL::DataType::FLOAT, false, 0, nullptr);
        m_opengl->VerifyNoError("Upload vertices failed.");

        // Upload normals (VEC3)
        upload += vec3s_bytes;
        m_opengl->BindBuffer(OpenGL::Target::ARRAY_BUFFER, m_buffer_objects[2]);
        m_opengl->BufferData(OpenGL::Target::ARRAY_BUFFER, vec3s_bytes, upload, OpenGL::Usage::STATIC_DRAW);
        m_opengl->EnableVertexAttribArray(normals_location);
        m_opengl->VertexAttribPointer(normals_location, 3, OpenGL::DataType::FLOAT, true, 0, nullptr);
        m_opengl->VerifyNoError("Upload normals failed.");

        // Upload tangents (VEC4)
        upload += vec3s_bytes;
        m_opengl->BindBuffer(OpenGL::Target::ARRAY_BUFFER, m_buffer_objects[3]);
        m_opengl->BufferData(OpenGL::Target::ARRAY_BUFFER, vec4s_bytes, upload, OpenGL::Usage::STATIC_DRAW);
        m_opengl->EnableVertexAttribArray(tangents_location);
        m_opengl->VertexAttribPointer(tangents_location, 4, OpenGL::DataType::FLOAT, false, 0, nullptr);
        m_opengl->VerifyNoError("Upload tangents failed.");

        // Upload UVs (VEC2)
        upload += vec4s_bytes;
        m_opengl->BindBuffer(OpenGL::Target::ARRAY_BUFFER, m_buffer_objects[4]);
        m_opengl->BufferData(OpenGL::Target::ARRAY_BUFFER, vec2s_bytes, upload, OpenGL::Usage::STATIC_DRAW);
        m_opengl->EnableVertexAttribArray(uvs_location);
        m_opengl->VertexAttribPointer(uvs_location, 2, OpenGL::DataType::FLOAT, false, 0, nullptr);
        m_opengl->VerifyNoError("Upload UVs failed.");

        // Unbind to prevent other modifications
        m_opengl->BindVertexArray(0);
        m_opengl->BindBuffer( OpenGL::Target::ELEMENT_ARRAY_BUFFER, 0);
        m_opengl->BindBuffer( OpenGL::Target::ARRAY_BUFFER, 0);
        m_opengl->VerifyNoError("load_mesh cleanup failed.");
    }

    MeshGL::Objects::~Objects() noexcept
    {
        m_opengl->DeleteTexture( &m_base_texture );
        m_opengl->DeleteTexture( &m_normals_texture );
        m_opengl->DeleteTexture( &m_orm_texture );
        m_opengl->DeleteTexture( &m_emissive_team_texture );

        m_opengl->DeleteVertexArrays(1u, &m_vertex_array_object);
        m_opengl->DeleteBuffers(4u, m_buffer_objects);

        m_count = 0u;
        m_vertex_array_object = 0u;
        m_buffer_objects[0] = 0u;
        m_buffer_objects[1] = 0u;
        m_buffer_objects[2] = 0u;
        m_buffer_objects[3] = 0u;
    }

    //--------------------------------------------------------------------------

    MeshGL::MeshGL(std::shared_ptr<OpenGL> gl, const std::string_view& file)
    {
        static_assert( sizeof(MeshHeader) == ((1 * 4) + (2 * 8) + (4 * 3) + (sizeof(float) * 15)),
                        "MeshHeader is not packed as expected.");

        if(not gl)
        {
            throw std::runtime_error("OpenGL pointer is nullptr.");
        }

        std::ifstream mesh_file;
        mesh_file.open(file.data(), std::ios::binary | std::ios::in);

        if (mesh_file.is_open())
        {
            std::vector<std::uint8_t> buffer;

            // Read header.
            read_bytes(mesh_file, sizeof(MeshHeader), buffer);
            auto header = *reinterpret_cast<const MeshHeader*>(buffer.data());

            if(header.header[0] != 'M' or
               header.header[1] != 'S' or
               header.header[2] != 'H' or
               header.header[3] != '2')
            {
                std::stringstream ss;
                ss << "File '" << file << "' does not appear to be a Mesh file.";
                throw std::runtime_error(ss.str());
            }

            auto mesh_objects = std::make_shared<MeshGL::Objects>(gl);

            // Read mesh buffer.
            read_bytes(mesh_file, header.mesh_bytes(), buffer);
            mesh_objects->load_mesh(header, buffer.data());

            // Read base color RGB
            read_bytes(mesh_file, header.base_color_texture_bytes(), buffer);
            mesh_objects->load_base_color_rgb(header, buffer.data());

            // Read normals RGB
            read_bytes(mesh_file, header.normal_texture_bytes(), buffer);
            mesh_objects->load_normal_rgb(header, buffer.data());

            // Read Occlusion Roughness Metallic RGB
            read_bytes(mesh_file, header.orm_texture_bytes(), buffer);
            mesh_objects->load_orm_rgb(header, buffer.data());

            // Read emissive/team RGBA
            read_bytes(mesh_file, header.emissive_texture_bytes(), buffer);
            mesh_objects->load_emissive_team_rgba(header, buffer.data());

            m_objects.swap(mesh_objects);
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to open mesh '" << file << "' for reading.";
            throw std::runtime_error(ss.str());
        }
    }

    MeshGL::~MeshGL() noexcept
    {
    }

    void MeshGL::draw()
    {
        m_objects->m_opengl->ActiveTexture( OpenGL::TextureTarget::TEXTURE3 );
        m_objects->m_opengl->BindTexture( OpenGL::TextureType::TEXTURE_2D, m_objects->m_emissive_team_texture );

        m_objects->m_opengl->ActiveTexture( OpenGL::TextureTarget::TEXTURE2 );
        m_objects->m_opengl->BindTexture(OpenGL::TextureType::TEXTURE_2D, m_objects->m_orm_texture );

        m_objects->m_opengl->ActiveTexture( OpenGL::TextureTarget::TEXTURE1 );
        m_objects->m_opengl->BindTexture( OpenGL::TextureType::TEXTURE_2D, m_objects->m_normals_texture );

        m_objects->m_opengl->ActiveTexture( OpenGL::TextureTarget::TEXTURE0 );
        m_objects->m_opengl->BindTexture( OpenGL::TextureType::TEXTURE_2D, m_objects->m_base_texture );

        m_objects->m_opengl->BindVertexArray(m_objects->m_vertex_array_object);
        m_objects->m_opengl->DrawElements( OpenGL::Mode::TRIANGLES, m_objects->m_count, m_objects->m_index_type, nullptr);

        m_objects->m_opengl->BindVertexArray(0u);
        m_objects->m_opengl->BindTexture( OpenGL::TextureType::TEXTURE_2D, 0u );
    }

    std::array<float, 3u> MeshGL::centre() const noexcept
    {
        return m_objects->m_centre_xyz;
    }

    float MeshGL::bounding_radius() const noexcept
    {
        return m_objects->m_bounding_radius;
    }

    //--------------------------------------------------------------------------

    std::size_t MeshHeader::mesh_bytes() const noexcept
    {
        auto index_bytes = index_size * index_count;
        auto vec2s_bytes = sizeof(float) * 2u * vertex_count;
        auto vec3s_bytes = sizeof(float) * 3u * vertex_count;
        auto vec4s_bytes = sizeof(float) * 4u * vertex_count;

        //  Indices     (index_count * 16 or 32 bit unsigned, see `index_size`)
        //  Positions   (vertex_count * 3 * float).
        //  Normals     (vertex_count * 3 * float).
        //  Tangents    (vertex_count * 4 * float) - 4th float is w, either +1 or -1.
        //  Texture UVs (vertex_count * 2 * float)

        return index_bytes + vec3s_bytes + vec3s_bytes + vec4s_bytes + vec2s_bytes;
    }

    std::size_t MeshHeader::base_color_texture_bytes() const noexcept
    {
        return 3u * base_color_size[0] * base_color_size[1];
    }

    std::size_t MeshHeader::orm_texture_bytes() const noexcept
    {
        return 3u * orm_size[0] * orm_size[1];
    }

    std::size_t MeshHeader::normal_texture_bytes() const noexcept
    {
        return 3u * normal_size[0] * normal_size[1];
    }

    std::size_t MeshHeader::emissive_texture_bytes() const noexcept
    {
        return 4u * emissive_team_size[0] * emissive_team_size[1];
    }
}