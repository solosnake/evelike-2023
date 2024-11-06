#ifndef SOLOSNAKE_GLTF_TO_MESH_HPP
#define SOLOSNAKE_GLTF_TO_MESH_HPP

#include <string_view>

namespace solosnake
{
    /// Opens the glTF file @a src_file and converts it to
    /// the internal mesh format and saves it as @a dst_file.
    /// Throws an @a Error in the event of a problem.
    void convert_glTF_to_mesh(const std::string_view& src_file,
                              const std::string_view& dst_file);
}

#endif