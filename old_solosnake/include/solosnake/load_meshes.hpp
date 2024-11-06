#ifndef solosnake_load_meshes_hpp
#define solosnake_load_meshes_hpp

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include "solosnake/filepath.hpp"

namespace solosnake
{
    class imesh;
    class ifilefinder;

    //! This struct is used when saving the meshes to file.
    struct meshdata
    {
        std::string name;
        std::string diffAndSpecName;
        std::string normalsAndEmissName;
        std::vector<std::uint16_t> indices;
        std::vector<float> vertices;
        std::vector<float> uvs;
        std::vector<float> tangents;
        std::vector<float> smoothednormals;
    };

    //! Intended to be a 'fast' lookup of a mesh in a file, this is an
    //! optimistic method which will assume the first matching geom and tex-info
    //! with matching names is the correct pair.
    //! Note especially that parent names are not checked.
    std::shared_ptr<imesh> load_mesh(
        const std::string& meshname,
        const filepath& url,
        const std::shared_ptr<ifilefinder>& texfinder );

    //! Creates or overwrites the named file. Saves an array of meshdata
    //! structs to a binary format file which can be read using
    //! load_mesh_datas.
    void save_mesh_datas(
        const filepath&,
        const std::vector<meshdata>& );

    //! Opens and reads the named binary file and returns its contents as an
    //! array of meshes. Internally these meshes will be meshdata structs.
    std::vector<std::shared_ptr<imesh>> load_mesh_datas(
                                         const filepath& url,
                                         const std::shared_ptr<ifilefinder>& texfinder );
}

#endif
