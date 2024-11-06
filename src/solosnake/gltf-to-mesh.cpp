#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "solosnake/decode-64.hpp"
#include "solosnake/gltf-to-mesh.hpp"
#include "solosnake/image.hpp"
#include "solosnake/maths.hpp"
#include "solosnake/nlohmann/json.hpp"
#include "solosnake/meshgl.hpp"

namespace solosnake
{
    namespace
    {
        struct VEC2 { float u, v; };
        struct VEC3 { float x, y, z; };
        struct VEC4 { float x, y, z, w; };

        VEC2 operator - (const VEC2& lhs, const VEC2& rhs) noexcept
        {
            return { lhs.u - rhs.u, lhs.v - rhs.v };
        }

        VEC3 operator - (const VEC3& lhs, const VEC3& rhs) noexcept
        {
            return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
        }

        VEC3 operator * (const VEC3& lhs, const float s) noexcept
        {
            return { lhs.x * s, lhs.y * s, lhs.z * s };
        }

        VEC3 normalized(const VEC3& v3)
        {
            const auto length = std::sqrt( (v3.x * v3.x) + (v3.y * v3.y) + (v3.z * v3.z) );
            return { v3.x / length, v3.y / length, v3.z / length };
        }

        /// @note Returns 1 if two *UNIT* vectors are completely aligned
        ///     (parallel), -1 if they're antiparallel, and zero if they're
        ///     normal to each other.
        float dot(const VEC3& lhs, const VEC3& rhs)
        {
            return ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z));
        }

        struct ImgSize
        {
            std::uint16_t width  {0u};
            std::uint16_t height {0u};
        };

        struct glTFMesh
        {
            Point3d                     centre                { 0.0f, 0.0f, 0.0f };
            float                       bounding_radius       { 0.0f };
            float                       base_color_factors[4] { 1.0f, 1.0f, 1.0f, 1.0f };
            float                       emissive_factors[3]   { 1.0f, 1.0f, 1.0f };
            float                       occlusion_factor      { 1.0f };
            float                       roughness_factor      { 1.0f };
            float                       metallic_factor       { 1.0f };
            float                       team_factor           { 1.0f };
            std::string                 base_color_texture;
            std::string                 metallic_roughness_texture;
            std::string                 occlusion_texture;
            std::string                 normals_texture;
            std::string                 emissive_team_texture;
            std::vector<VEC3>           positions;
            std::vector<VEC3>           normals;
            std::vector<VEC4>           tangents;
            std::vector<VEC2>           tex_coords;
            std::vector<std::uint16_t>  u16_indices;
            std::vector<std::uint32_t>  u32_indices;

            std::size_t index_count() const noexcept
            {
                return u16_indices.empty() ? u32_indices.size() : u16_indices.size();
            }

            std::uint32_t index(const std::size_t n) const
            {
                return u16_indices.empty() ? u32_indices.at(n) : u16_indices.at(n);
            }

            std::uint32_t index_type_size() const noexcept
            {
                return (u16_indices.size() > 0u) ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
            }
        };

        enum class ViewType
        {
            Indices, Positions, Normals, Tangents, TextureCoords
        };

        struct Buffer_view
        {
            const void* operator[] (std::size_t n) const
            {
                return &buffer->at((accessor_offset + view_offset) + (n * view_stride));
            }

            std::size_t size() const noexcept
            {
                return count;
            }

            std::uint64_t                               count;
            std::uint64_t                               accessor_offset;
            std::uint64_t                               view_offset;
            std::uint64_t                               view_stride;
            std::shared_ptr<std::vector<std::uint8_t>>  buffer;
            int                                         component_type;
            ViewType                                    view_type;
        };

        struct Summed_tangent
        {
            void operator+=(const VEC4& v3)
            {
                tangent.x += v3.x;
                tangent.y += v3.y;
                tangent.z += v3.z;
                tangent.w = v3.w;
                ++count;
            }

            VEC4 get_averaged_tangent() const
            {
                assert(count > 0u);
                auto x = tangent.x / count;
                auto y = tangent.y / count;
                auto z = tangent.z / count;
                auto n = normalized( VEC3{x, y, z} );
                return { n.x, n.y, n.z, tangent.w };
            }

            VEC4            tangent {0.0f, 0.0f, 0.0f, 1.0f};
            std::uint32_t   count   {0u};
        };

        constexpr auto FLOAT_COMPONENT_TYPE{5126};
        constexpr auto U16_COMPONENT_TYPE  {5123};
        constexpr auto U32_COMPONENT_TYPE  {5125};

        void validate_type(ViewType t, const std::string& text)
        {
            switch(t)
            {
            case ViewType::Indices:
                if("SCALAR" != text)
                {
                    throw std::runtime_error("Unsupported mesh index type - expected SCALAR");
                }
                break;

            case ViewType::Positions:
            case ViewType::Normals:
                if("VEC3" != text)
                {
                    throw std::runtime_error("Unsupported mesh type - expected VEC3");
                }
                break;

            case ViewType::TextureCoords:
                if("VEC2" != text)
                {
                    throw std::runtime_error("Unsupported mesh UV type - expected VEC2");
                }
                break;

            case ViewType::Tangents:
                if("VEC4" != text)
                {
                    throw std::runtime_error("Unsupported mesh tangent type - expected VEC4");
                }
                break;

            default:
                // std::unreachable();
                break;
            }
        }

        void validate_component_type(ViewType t, int componentType)
        {
            switch(t)
            {
            case ViewType::Indices:
                if(U16_COMPONENT_TYPE != componentType and U32_COMPONENT_TYPE != componentType)
                {
                    throw std::runtime_error("Unsupported mesh index componentType");
                }
                break;

            case ViewType::Positions:
            case ViewType::Normals:
            case ViewType::TextureCoords:
            case ViewType::Tangents:
                if(FLOAT_COMPONENT_TYPE != componentType)
                {
                    throw std::runtime_error("Unsupported mesh componentType");
                }
                break;

            default:
                // std::unreachable();
                break;
            }
        }

        void validate_mesh(const glTFMesh& mesh)
        {
            if(0u != (mesh.index_count() % 3u))
            {
                throw std::runtime_error("Indices are not a multiple of 3 (TRIANGLES).");
            }

            if(mesh.positions.size() != mesh.normals.size())
            {
                throw std::runtime_error("Positions/normals count mismatch.");
            }

            if(mesh.positions.size() != mesh.tangents.size())
            {
                throw std::runtime_error("Positions/tangents count mismatch.");
            }

            if(mesh.positions.size() != mesh.tex_coords.size())
            {
                throw std::runtime_error("Positions/texture-coords count mismatch.");
            }

            if(mesh.positions.empty())
            {
                throw std::runtime_error("No positions in mesh.");
            }

            if(0u == mesh.index_count())
            {
                throw std::runtime_error("No indices in mesh.");
            }

            for(auto i=0u; i < mesh.index_count(); ++i)
            {
                if(mesh.index(i) >= mesh.positions.size())
                {
                    throw std::runtime_error("Bad index value.");
                }
            }
        }

        void set_mesh_bounding_sphere(glTFMesh& mesh, const nlohmann::json& js)
        {
            const auto positions_index = js["meshes"][0]["primitives"][0]["attributes"]["POSITION"].get<int>();
            const auto& lowers = js.at("accessors")[positions_index].at("min");
            const auto& uppers = js.at("accessors")[positions_index].at("max");
            const auto lower_x = lowers.at(0).get<float>();
            const auto lower_y = lowers.at(1).get<float>();
            const auto lower_z = lowers.at(2).get<float>();
            const auto upper_x = uppers.at(0).get<float>();
            const auto upper_y = uppers.at(1).get<float>();
            const auto upper_z = uppers.at(2).get<float>();
            mesh.centre = { (lower_x + upper_x) * 0.5f,
                            (lower_y + upper_y) * 0.5f,
                            (lower_z + upper_z) * 0.5f };
            mesh.bounding_radius = distance(mesh.centre,
                                            Point3d{lower_x,lower_y, lower_z});
        }

        void fill(const Buffer_view& view, std::vector<VEC2>& v2)
        {
            v2.resize(view.count);
            for(auto i=0u; i < view.size(); ++i)
            {
                v2[i] = *static_cast<const VEC2*>(view[i]);;
            }
        }

        void fill(const Buffer_view& view, std::vector<VEC3>& v3)
        {
            v3.resize(view.count);
            for(auto i=0u; i < view.size(); ++i)
            {
                v3[i] = *static_cast<const VEC3*>(view[i]);;
            }
        }

        void fill(const Buffer_view& view, std::vector<VEC4>& v4)
        {
            v4.resize(view.count);
            for(auto i=0u; i < view.size(); ++i)
            {
                v4[i] = *static_cast<const VEC4*>(view[i]);;
            }
        }

        void fill(const Buffer_view& view, std::vector<std::uint16_t>& u16s)
        {
            u16s.resize(view.count);
            for(auto i=0u; i < view.size(); ++i)
            {
                u16s[i] = *static_cast<const std::uint16_t*>(view[i]);;
            }
        }

        void fill(const Buffer_view& view, std::vector<std::uint32_t>& u32s)
        {
            u32s.resize(view.count);
            for(auto i=0u; i < view.size(); ++i)
            {
                u32s[i] = *static_cast<const std::uint32_t*>(view[i]);;
            }
        }

        void fill_mesh(glTFMesh& mesh, const Buffer_view& view)
        {
            switch(view.view_type)
            {
            case ViewType::Indices:
                if(view.component_type == U16_COMPONENT_TYPE)
                {
                    fill(view, mesh.u16_indices);
                }
                else
                {
                    fill(view, mesh.u32_indices);
                }
                break;

            case ViewType::Positions:
                fill(view, mesh.positions);
                break;

            case ViewType::Normals:
                fill(view, mesh.normals);
                break;

            case ViewType::Tangents:
                fill(view, mesh.tangents);
                break;

            case ViewType::TextureCoords:
                fill(view, mesh.tex_coords);
                break;

            default:
                // std::unreachable();
                break;
            }
        }

        std::shared_ptr<std::vector<std::uint8_t>> read_buffer(const nlohmann::json& buffer,
                                                               const std::filesystem::path& root)
        {
            std::shared_ptr<std::vector<std::uint8_t>>  bytes;

            const auto byte_length = buffer.at("byteLength").get<std::size_t>();
            const auto& uri = buffer.at("uri").get<std::string>();

            const std::string_view token("data:application/octet-stream;base64,");

            if(uri.starts_with(token))
            {
                const auto d64 = uri.data() + token.length();
                bytes = std::make_shared<std::vector<std::uint8_t>>( decode_64(d64) );
            }
            else
            {
                auto uri_path = std::filesystem::path(uri);

                if( not std::filesystem::exists(uri_path) )
                {
                    // See if we can find it relative to the glTF file.
                    uri_path = std::filesystem::absolute( root / uri_path );
                }

                std::ifstream file( uri_path.c_str(), std::ios::binary | std::ios::in );

                if(file.is_open())
                {
                    file.seekg (0, std::ios::end);
                    const auto length = file.tellg();
                    file.seekg (0, std::ios::beg);

                    bytes = std::make_shared<std::vector<std::uint8_t>>( length, std::uint8_t{0u} );

                    file.read( reinterpret_cast<std::ifstream::char_type*>(bytes->data()), bytes->size());
                    file.close();
                }
                else
                {
                    std::stringstream ss;
                    ss << "Unable to open uri buffer file '" << uri << "'.";
                    throw std::runtime_error(ss.str());
                }
            }

            if(bytes->size() != byte_length)
            {
                throw std::runtime_error("Decoded uri buffer was not expected byte length.");
            }

            return bytes;
        }

        bool has_tangents(const nlohmann::json& js)
        {
            return js.at("meshes").at(0).at("primitives").at(0).at("attributes").contains("TANGENT");
        }

        void normalise(glTFMesh& mesh)
        {
            for(auto i=0u; i < mesh.normals.size(); ++i)
            {
                mesh.normals[i] = normalized( mesh.normals[i] );
            }
        }

        void make_tangents(glTFMesh& mesh)
        {
            // XYZW vertex tangents where the XYZ portion is normalized, and the
            // W component is a sign value (-1 or +1) indicating handedness of
            // the tangent basis;
            // The bitangent vectors MUST be computed by taking the cross
            // product of the normal and tangent XYZ vectors and multiplying it
            // against the W component of the tangent:
            //      bitangent = cross(normal.xyz, tangent.xyz) * tangent.w.
            // See https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html

            // See http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#tangent-and-bitangent

            std::vector<Summed_tangent> summed_tangents;
            mesh.tangents.resize(mesh.positions.size());
            summed_tangents.resize(mesh.positions.size());

            const auto n = mesh.index_count();
            for(auto i=0u; i < n; i += 3u)
            {
                const auto i0 = mesh.index(i);
                const auto i1 = mesh.index(i+1u);
                const auto i2 = mesh.index(i+2u);

                // Shortcuts for vertices
                const auto v0 = mesh.positions.at(i0);
                const auto v1 = mesh.positions.at(i1);
                const auto v2 = mesh.positions.at(i2);

                // Shortcuts for UVs
                const auto uv0 = mesh.tex_coords.at(i0);
                const auto uv1 = mesh.tex_coords.at(i1);
                const auto uv2 = mesh.tex_coords.at(i2);

                // Edges of the triangle : position delta
                const auto edge1 = v1 - v0;
                const auto edge2 = v2 - v0;

                // UV delta
                const auto deltaUV1 = uv1 - uv0;
                const auto deltaUV2 = uv2 - uv0;

                // Calculate VEC3 tangent.
                const auto r = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV1.v * deltaUV2.u);
                const auto t = ((edge1 * deltaUV2.v) - (edge2 * deltaUV1.v)) * r;

                // We dont need bi-tangent.
                // auto bi = ((edge2 * deltaUV1.u) - (edge1 * deltaUV2.u)) * r;

                // Check each tangent to see if it is facing in same direction
                // as the vertex normal. If not, use w as -1 to flip it.
                const auto n0 = mesh.normals.at(i0);
                const auto n1 = mesh.normals.at(i1);
                const auto n2 = mesh.normals.at(i2);

                // Is tangent facing away from face normal?
                const auto nt = normalized(t);
                const float w0 = (dot(n0,nt) > 0.0f) ? 1.0f : -1.0f;
                const float w1 = (dot(n1,nt) > 0.0f) ? 1.0f : -1.0f;
                const float w2 = (dot(n2,nt) > 0.0f) ? 1.0f : -1.0f;

                // Sum non-normalised tangents: larger triangles will contribute
                // more to average. Normalisation is done later.
                summed_tangents.at(i0) += VEC4{ t.x, t.y, t.z, w0 };
                summed_tangents.at(i1) += VEC4{ t.x, t.y, t.z, w1 };
                summed_tangents.at(i2) += VEC4{ t.x, t.y, t.z, w2 };
            }

            // Store final averaged tangent.
            for(auto i=0u; i < summed_tangents.size(); ++i)
            {
                mesh.tangents[i] = summed_tangents[i].get_averaged_tangent();
            }
        }

        Buffer_view get_buffer_view(const nlohmann::json& js,
                                    const std::filesystem::path& gltf_root,
                                    std::map<int, std::shared_ptr<std::vector<std::uint8_t>>>& buffers,
                                    ViewType t)
        {
            const auto& primitive = js.at("meshes").at(0).at("primitives").at(0);

            int index = 0;
            switch(t)
            {
            case ViewType::Indices:
                index = primitive.at("indices").get<int>();
            break;

            case ViewType::Positions:
                index = primitive.at("attributes").at("POSITION").get<int>();
            break;

            case ViewType::Normals:
                index = primitive.at("attributes").at("NORMAL").get<int>();
            break;

            case ViewType::Tangents:
                index = primitive.at("attributes").at("TANGENT").get<int>();
            break;

            case ViewType::TextureCoords:
                index = primitive.at("attributes").at("TEXCOORD_0").get<int>();
            break;

            default:
                throw std::runtime_error("Unhandled ViewType");
            break;
            }

            const auto& accessor = js.at("accessors")[index];
            const auto componentType = accessor.at("componentType").get<int>();
            const auto vec_type = accessor.at("type").get<std::string>();

            validate_type(t, vec_type);
            validate_component_type(t, componentType);

            const auto& buffer_view = js.at("bufferViews").at( accessor.at("bufferView").get<int>() );
            const auto& buffer      = js.at("buffers").at( buffer_view.at("buffer").get<int>() );

            if(not buffers.contains(index))
            {
                buffers[index] = read_buffer(buffer, gltf_root);
            }

            std::uint64_t type_size = 0u;
            switch(t)
            {
            case ViewType::Indices:
                type_size = (U16_COMPONENT_TYPE == componentType)
                          ? sizeof(std::uint16_t)
                          : sizeof(std::uint32_t);
            break;

            case ViewType::Positions:
            case ViewType::Normals:
                type_size = sizeof(VEC3);
            break;

            case ViewType::Tangents:
                type_size = sizeof(VEC4);
            break;

            case ViewType::TextureCoords:
                type_size = sizeof(VEC2);
            break;

            default:
                // Unreachable
            break;
            }


            Buffer_view view;
            view.count           = accessor.at("count").get<unsigned int>();
            view.component_type  = componentType;
            view.view_type       = t;
            view.accessor_offset = accessor.value("byteOffset", 0);
            view.view_offset     = buffer_view.value("byteOffset", 0);
            view.view_stride     = buffer_view.value("byteStride", type_size);
            view.buffer          = buffers[index];

            return view;
        }

        /// Throws if the named file does not exist.
        void verify_file_exists(const std::string_view& file, const char* name)
        {
            std::stringstream ss;

            if(file.empty())
            {
                ss << name << " file name is empty.";
                throw std::runtime_error(ss.str());
            }

            if(not std::filesystem::exists(file))
            {
                ss << name << " file '" << file << "' not found.";
                throw std::runtime_error(ss.str());
            }
        }

        /// Throws if the image width or height is greater than uint16, or zero.
        void check_image_size(const Image& img)
        {
            if(img.width() == 0u or img.height() == 0u )
            {
                throw std::runtime_error("Image is empty.");
            }

            if (img.width() > std::numeric_limits<std::uint16_t>::max())
            {
                throw std::runtime_error("Image is too wide.");
            }

            if (img.height() > std::numeric_limits<std::uint16_t>::max())
            {
                throw std::runtime_error("Image is too long.");
            }
        }


        ImgSize write_orm_rgb(std::ofstream& mesh_file,
                              const std::string_view& occlusion_texture,
                              const std::string_view& metallic_roughness_texture)
        {
            if(occlusion_texture.empty() and metallic_roughness_texture.empty())
            {
                // No ORM textures.
                return { 0u, 0u };
            }

            std::optional<Image> occlusion;
            std::optional<Image> metallic_roughness;

            // Load the metallic roughness, if present. This is always the
            // image we will write. We will add occlusion data into this.
            if(not metallic_roughness_texture.empty())
            {
                verify_file_exists(metallic_roughness_texture, "Metallic-roughness");
                metallic_roughness.emplace(metallic_roughness_texture, Image::Format::BGR);
                check_image_size(metallic_roughness.value());
            }

            if(not occlusion_texture.empty())
            {
                if(occlusion_texture == metallic_roughness_texture)
                {
                    // Skip loading occlusion: its already in the
                    // metallic-roughness image red channel.
                }
                else
                {
                    verify_file_exists(occlusion_texture, "Occlusion");
                    occlusion.emplace(occlusion_texture, Image::Format::BGR);
                    check_image_size(occlusion.value());
                }
            }

            // At least one ORM texture is present. If we have occlusion, merge
            // it into metallic_roughness. If we dont have occlusion we just
            // use metallic_roughness.
            if(occlusion)
            {
                if(metallic_roughness)
                {
                    if((occlusion->width()  == metallic_roughness->width()) and
                       (occlusion->height() == metallic_roughness->height()))
                    {
                        // Copy RED occlusion to RED metallic-roughness
                        // (BLUE=metal, GREEN=roughness).
                        Image::copy_layer( *occlusion, 2, *metallic_roughness, 2 );
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "Occlusion and metallic-roughness textures are not identical dimensions.";
                        ss << " (" << occlusion_texture << " and " << metallic_roughness_texture << ").";
                        throw std::runtime_error(ss.str());
                    }
                }
                else
                {
                    // Occlusion but no metallic-roughness: use occlusion as
                    // metallic-roughness.
                    metallic_roughness.swap(occlusion);
                }
            }
            else
            {
                // No occlusion, but we know we at least have metallic-roughness.
            }

            mesh_file.write( reinterpret_cast<const char*>(metallic_roughness->pixels()),
                             metallic_roughness->bytes_size() );

            return { static_cast<std::uint16_t>(metallic_roughness->width()),
                     static_cast<std::uint16_t>(metallic_roughness->height()) };
        }

        ImgSize write_image(std::ofstream& mesh_file,
                            const std::string_view& texture,
                            const char* name,
                            const Image::Format format)
        {
            if(texture.empty())
            {
                return { 0u, 0u };
            }
            else
            {
                verify_file_exists(texture, name);
                Image img(texture, format);
                check_image_size(img);
                mesh_file.write( reinterpret_cast<const char*>(img.pixels()), img.bytes_size() );
                return { static_cast<std::uint16_t>(img.width()), static_cast<std::uint16_t>(img.height()) };
            }
        }

        void save_mesh(const glTFMesh& mesh, const std::string_view& dst_file)
        {
            std::ofstream mesh_file(dst_file.data(), std::ios::binary | std::ios::out);
            if (mesh_file.is_open())
            {
                // Throw when problems are encountered.
                mesh_file.exceptions(std::ifstream::failbit|std::ifstream::badbit);

                MeshHeader header;
                static_assert( sizeof(header) == ((1 * 4) + (2 * 8) + (4 * 3) + (sizeof(float) * 15)),
                               "MeshHeader is not packed as expected.");

                header.index_count     = static_cast<std::uint32_t>(mesh.index_count());
                header.index_size      = mesh.index_type_size();
                header.vertex_count    = static_cast<std::uint32_t>(mesh.positions.size());
                header.centre[0]       = mesh.centre.x;
                header.centre[1]       = mesh.centre.y;
                header.centre[2]       = mesh.centre.z;
                header.bounding_radius = mesh.bounding_radius;

                header.base_color_factors[0] = mesh.base_color_factors[0];
                header.base_color_factors[1] = mesh.base_color_factors[1];
                header.base_color_factors[2] = mesh.base_color_factors[2];
                header.base_color_factors[3] = mesh.base_color_factors[3];

                header.emissive_factors[0] = mesh.emissive_factors[0];
                header.emissive_factors[1] = mesh.emissive_factors[1];
                header.emissive_factors[2] = mesh.emissive_factors[2];

                header.occlusion_factor = mesh.occlusion_factor;
                header.roughness_factor = mesh.roughness_factor;
                header.metallic_factor  = mesh.metallic_factor;
                header.team_factor      = mesh.team_factor;

                // Header
                mesh_file.write( reinterpret_cast<const char*>(&header), sizeof(header) );

                //  Indices     (index_count * 16 or 32 bit unsigned, see `index_size`)
                //  Positions   (vertex_count * 3 * float).
                //  Normals     (vertex_count * 3 * float).
                //  Tangents    (vertex_count * 4 * float) - 4th float is w, either +1 or -1.
                //  Texture UVs (vertex_count * 2 * float)
                if( mesh.index_type_size() == sizeof(std::uint16_t) )
                {
                    mesh_file.write( reinterpret_cast<const char*>(mesh.u16_indices.data()),
                                     mesh.u16_indices.size() * sizeof(std::uint16_t) );
                }
                else
                {
                    mesh_file.write( reinterpret_cast<const char*>(mesh.u32_indices.data()),
                                     mesh.u32_indices.size() * sizeof(std::uint32_t) );
                }

                mesh_file.write( reinterpret_cast<const char*>(mesh.positions.data()),
                                 mesh.positions.size() * sizeof(VEC3) );

                mesh_file.write( reinterpret_cast<const char*>(mesh.normals.data()),
                                 mesh.normals.size() * sizeof(VEC3) );

                mesh_file.write( reinterpret_cast<const char*>(mesh.tangents.data()),
                                 mesh.tangents.size() * sizeof(VEC4) );

                mesh_file.write( reinterpret_cast<const char*>(mesh.tex_coords.data()),
                                 mesh.tex_coords.size() * sizeof(VEC2) );

                auto base_size      = write_image( mesh_file, mesh.base_color_texture, "Base", Image::Format::BGR );
                auto normals_size   = write_image( mesh_file, mesh.normals_texture, "Normals", Image::Format::BGR );
                auto orm_size       = write_orm_rgb(mesh_file, mesh.occlusion_texture, mesh.metallic_roughness_texture);
                auto emissive_team_size = write_image( mesh_file, mesh.emissive_team_texture, "Emissive-team", Image::Format::BGRA );

                // Update the header with the correct image sizes.
                header.base_color_size[0] = base_size.width;
                header.base_color_size[1] = base_size.height;
                header.orm_size[0]        = orm_size.width;
                header.orm_size[1]        = orm_size.height;
                header.normal_size[0]     = normals_size.width;
                header.normal_size[1]     = normals_size.height;
                header.emissive_team_size[0] = emissive_team_size.width;
                header.emissive_team_size[1] = emissive_team_size.height;

                // Write corrected header.
                mesh_file.seekp(0u, std::ios_base::beg);
                mesh_file.write( reinterpret_cast<const char*>(&header), sizeof(header) );

                // Flush to disk. May stall but this code is not time critical.
                mesh_file.flush();
            }
            else
            {
                std::stringstream ss;
                ss << "Failed to open '" << dst_file << "' for writing.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    //--------------------------------------------------------------------------

    void convert_glTF_to_mesh(const std::string_view& src_file,
                              const std::string_view& dst_file)
    {
        if (not std::filesystem::exists(src_file))
        {
            throw std::runtime_error(std::string("File not found: ") + src_file.data());
        }

        try
        {
            // Files mentioned in the glTF might be relative to the glTF file:
            // get the root of the glTF file location.
            const auto file_root = std::filesystem::absolute(src_file).remove_filename();

            // Parse the glTF as JSON.
            std::ifstream glTF(src_file.data());
            const auto js = nlohmann::json::parse(glTF);
            glTF.close();

            if(1u != js.at("meshes").size())
            {
                throw std::runtime_error(src_file.data() + std::string(" has more than one mesh."));
            }

            if(1u != js["meshes"][0].at("primitives").size())
            {
                throw std::runtime_error(src_file.data() + std::string(" mesh has more than one primitive."));
            }

            // 0 POINTS, 1 LINES, 2 LINE_LOOP, 3 LINE_STRIP 4 TRIANGLES,
            // 5 TRIANGLE_STRIP 6 TRIANGLE_FAN. We only support TRIANGLES:
            if(js["meshes"][0]["primitives"][0].contains("mode"))
            {
                // Mode is specified. It is 4 by default (TRIANGLES).
                if(4 != js["meshes"][0]["primitives"][0].at("mode").get<int>())
                {
                    throw std::runtime_error(src_file.data()
                                             + std::string(" mesh primitive mode is not 4 (TRIANGLES)."));
                }
            }

            // Gather all the image paths that are file paths, ignoring
            // data buffers.
            std::map<int, std::string> image_uris;
            for(auto i=0u; i < js.at("images").size(); ++i)
            {
                if(js["images"][i].contains("uri"))
                {
                    const auto& uri = js["images"][i]["uri"].get<std::string>();

                    if(not uri.empty())
                    {
                        if(uri.starts_with("data:"))
                        {
                            // Skip this, this is a data buffer.
                        }
                        else
                        {
                            auto uri_path = std::filesystem::path(uri);

                            if( std::filesystem::exists(uri_path) )
                            {
                                image_uris[i] = uri_path.string();
                            }
                            else
                            {
                                // See if we can find it relative to the glTF file.
                                uri_path = std::filesystem::absolute( file_root / uri_path ).string();
                                image_uris[i] = uri_path.string();
                            }
                        }
                    }
                }
            }

            // Gather the buffer views.
            std::map<int, std::shared_ptr<std::vector<std::uint8_t>>> buffers;
            auto indices   = get_buffer_view(js, file_root, buffers, ViewType::Indices);
            auto positions = get_buffer_view(js, file_root, buffers, ViewType::Positions);
            auto normals   = get_buffer_view(js, file_root, buffers, ViewType::Normals);
            auto uv_coords = get_buffer_view(js, file_root, buffers, ViewType::TextureCoords);

            glTFMesh mesh;

            const auto material_index  = js["meshes"][0]["primitives"][0].at("material").get<int>();
            const auto& material = js.at("materials").at(material_index);

            if(not material.contains("pbrMetallicRoughness"))
            {
                throw std::runtime_error(src_file.data() + std::string(" has no 'pbrMetallicRoughness'."));
            }

            // Examine `pbrMetallicRoughness`
            const auto& pbr = material.at("pbrMetallicRoughness");

            if(pbr.contains("baseColorFactor"))
            {
                mesh.base_color_factors[0] = pbr["baseColorFactor"].at(0).get<float>();
                mesh.base_color_factors[1] = pbr["baseColorFactor"].at(1).get<float>();
                mesh.base_color_factors[2] = pbr["baseColorFactor"].at(2).get<float>();
                mesh.base_color_factors[3] = pbr["baseColorFactor"].at(3).get<float>();
            }

            if(pbr.contains("roughnessFactor"))
            {
                mesh.roughness_factor = pbr["roughnessFactor"].get<float>();
            }

            if(pbr.contains("metallicFactor"))
            {
                mesh.metallic_factor = pbr["metallicFactor"].get<float>();
            }

            // Local functor to retrieve the texture name from pbr if it exists.
            // Throws if it is not a file path, else returns name, or empty string.
            auto get_pbr_texture = [&](const std::string& name) -> std::string
            {
                if(pbr.contains(name))
                {
                    auto texture_index = pbr[name].at("index").get<int>();
                    if(image_uris.contains(texture_index))
                    {
                        return image_uris.at(texture_index);
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "No image file path for '" << name << "'.";
                        throw std::runtime_error(ss.str());
                    }
                }

                // Return empty string (no-op result).
                return std::string();
            };

            // Local functor to retrieve the texture name from material if it exists.
            // Throws if it is not a file path, else returns name, or empty string.
            auto get_texture = [&](const std::string& name) -> std::string
            {
                if(material.contains(name))
                {
                    auto texture_index = material[name].at("index").get<int>();
                    if(image_uris.contains(texture_index))
                    {
                        return image_uris.at(texture_index);
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "No image file path for '" << name << "'.";
                        throw std::runtime_error(ss.str());
                    }
                }

                // Return empty string (no-op result).
                return std::string();
            };

            // PBR textures: base color (we only use RGB),
            // and metallic-roughness (blue = metal, green = roughness)
            mesh.base_color_texture         = get_pbr_texture("baseColorTexture");
            mesh.metallic_roughness_texture = get_pbr_texture("metallicRoughnessTexture");

            // Material textures.
            mesh.emissive_team_texture      = get_texture("emissiveTexture");
            mesh.occlusion_texture          = get_texture("occlusionTexture");
            mesh.normals_texture            = get_texture("normalTexture");

            fill_mesh(mesh, indices);
            fill_mesh(mesh, positions);
            fill_mesh(mesh, normals);
            fill_mesh(mesh, uv_coords);

            set_mesh_bounding_sphere(mesh, js);

            // Ensure all normals are unit length.
            normalise(mesh);

            // Tangents may not be present: if missing, we calculate them:
            if(has_tangents(js))
            {
                auto tangents  = get_buffer_view(js, file_root, buffers, ViewType::Tangents);
                fill_mesh(mesh, tangents);
            }
            else
            {
                make_tangents(mesh);
            }

            // Check mesh is valid:
            validate_mesh(mesh);

            // Write mesh to file.
            save_mesh(mesh, dst_file);
        }
        catch (const nlohmann::json::exception &e)
        {
            std::stringstream ss;
            ss << "Error converting '" << src_file << "'. "
               << "JSON error : '" << e.what() << "'.";
            throw std::runtime_error(ss.str());
        }
        catch(const std::exception& e)
        {
            std::stringstream ss;
            ss << "Error converting '" << src_file << "'. " << e.what();
            throw std::runtime_error(ss.str());
        }
    }
}