#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include "solosnake/testing/testing.hpp"
#include "solosnake/gltf-to-mesh.hpp"
#include "solosnake/meshgl.hpp"
#include "solosnake/tga.hpp"

using namespace solosnake;

namespace
{
    /// Resizes the buffer and reads into it n bytes from f.
    void read_bytes(std::ifstream& f, std::size_t n, std::vector<std::uint8_t>& buffer)
    {
        buffer.resize(n, char(0));
        f.read(reinterpret_cast<char*>(buffer.data()), n);
    }
}

TEST( convert_glTF_to_mesh, SciFiHelmet )
{
    try
    {
        auto file = "SciFiHelmet.msh";

        convert_glTF_to_mesh("./test-assets/SciFiHelmet.gltf", file);

        static_assert( sizeof(MeshHeader) == ((1 * 4) + (2 * 8) + (4 * 3) + (sizeof(float) * 15)),
                       "MeshHeader is not packed as expected.");

        std::ifstream mesh_file;
        mesh_file.open(file, std::ios::binary | std::ios::in);

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

            // Read mesh buffer.
            read_bytes(mesh_file, header.mesh_bytes(), buffer);

            // Read base color RGB
            read_bytes(mesh_file, header.base_color_texture_bytes(), buffer);
            EXPECT_TRUE( buffer.size() == (3u * header.base_color_size[0] * header.base_color_size[1]) );
            save_bgr_tga(header.base_color_size[0], header.base_color_size[1], buffer.data(), false,
                         "SciFiHelmet_base.tga");

            // Read normals RGB
            read_bytes(mesh_file, header.normal_texture_bytes(), buffer);
            save_bgr_tga(header.normal_size[0], header.normal_size[1], buffer.data(), false,
                "SciFiHelmet_normals.tga");

            // Read Occlusion Roughness Metallic RGB
            read_bytes(mesh_file, header.orm_texture_bytes(), buffer);
            save_bgr_tga(header.orm_size[0], header.orm_size[1], buffer.data(), false,
                         "SciFiHelmet_ORM.tga");

            // Read emissive/team RGBA
            read_bytes(mesh_file, header.emissive_texture_bytes(), buffer);
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to open mesh '" << file << "' for reading.";
            throw std::runtime_error(ss.str());
        }

    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}