#ifndef SOLOSNAKE_MESHGL_HPP
#define SOLOSNAKE_MESHGL_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include "solosnake/opengl.hpp"
#include "solosnake/shadergl.hpp"

namespace solosnake
{
    /// Assumes in the associated shader that the vertex shader will have the
    /// following data and layout:
    ///
    ///     layout(location = 0) in vec3 positions
    ///     layout(location = 1) in vec3 normals
    ///     layout(location = 2) in vec4 tangents: 4th float either +1 or -1
    ///     layout(location = 3) in vec2 uvs
    ///
    /// XYZW vertex tangents where the XYZ portion is normalized, and the
    /// W component is a sign value (-1 or +1) indicating handedness of
    /// the tangent basis;
    /// The bitangent vectors MUST be computed by taking the cross
    /// product of the normal and tangent XYZ vectors and multiplying it
    /// against the W component of the tangent:
    ///      bitangent = cross(normal.xyz, tangent.xyz) * tangent.w.
    class MeshGL
    {
    public:

        explicit MeshGL(std::shared_ptr<OpenGL> gl,
                        const std::string_view& file);

        ~MeshGL() noexcept;

        void draw();

        std::array<float,3u> centre() const noexcept;

        float   bounding_radius() const noexcept;

    private:
        class Objects;
        std::shared_ptr<Objects> m_objects;
    };

    /// Start of the mesh binary files. The data follows this in the following
    /// order:
    ///  Indices     (index_count * 16 or 32 bit unsigned, see `index_size`)
    ///  Positions   (vertex_count * 3 * float).
    ///  Normals     (vertex_count * 3 * float).
    ///  Tangents    (vertex_count * 4 * float) - 4th float is w, either +1 or -1.
    ///  Texture UVs (vertex_count * 2 * float)
    ///
    /// After the mesh data follows the texture data. Each mesh uses PBR - Physically
    /// Based Rendering, and has the following textures:
    ///
    /// Base BGR
    /// Normals BGR
    /// Occlusion Roughness Metallic BGR
    /// Emissive / Team BGRA
    ///
    /// Occlusion Roughness Metallic
    /// ----------------------------
    /// 3 RGB
    /// R: Occlusion.
    /// G: Roughness
    /// B: Metalness
    ///
    /// Emissive
    /// --------
    /// RGB: Emissive Color
    /// A: Team (multiplied by a team color)
    ///
    struct MeshHeader
    {
        std::uint8_t  header[4]             { 'M', 'S', 'H', '2' };
        float         centre[3]             { 0.0f, 0.0f, 0.0f };
        float         bounding_radius       { 0.0f };
        std::uint32_t index_size            { sizeof(std::uint16_t) }; // Or sizeof(uint32).
        std::uint32_t index_count           { 0u };
        std::uint32_t vertex_count          { 0u };
        std::uint16_t base_color_size[2]    { 0u, 0u }; // Pixels width, height.
        std::uint16_t orm_size[2]           { 0u, 0u }; // Pixels width, height.
        std::uint16_t normal_size[2]        { 0u, 0u }; // Pixels width, height.
        std::uint16_t emissive_team_size[2] { 0u, 0u }; // Pixels width, height.
        float         base_color_factors[4] { 1.0f, 1.0f, 1.0f, 1.0f };
        float         emissive_factors[3]   { 1.0f, 1.0f, 1.0f };
        float         occlusion_factor      { 1.0f };
        float         roughness_factor      { 1.0f };
        float         metallic_factor       { 1.0f };
        float         team_factor           { 1.0f };

        std::size_t   mesh_bytes() const noexcept;
        std::size_t   base_color_texture_bytes() const noexcept;
        std::size_t   orm_texture_bytes() const noexcept;
        std::size_t   normal_texture_bytes() const noexcept;
        std::size_t   emissive_texture_bytes() const noexcept;
    };

}

#endif