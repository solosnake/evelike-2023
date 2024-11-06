#ifndef solosnake_meshmaker_hpp
#define solosnake_meshmaker_hpp

#include <string>
#include <vector>
#include "solosnake/filepath.hpp"

namespace solosnake
{
    struct mesh_making_params
    {
        std::string              inputFileName;
        std::string              outputModelName;
        solosnake::filepath      outputMeshFilepath;
        solosnake::filepath      outputModelFilepath;
        solosnake::filepath      texturesOutDirectory;
        std::vector<std::string> textureSourceDirectories;
        float                    scalingFactor;
        float                    translation[3];
        bool                     use3DSMaxCoords;   // If true x = -x, y = z, z = y.

    };

    void make_mesh( const mesh_making_params&, const bool showmesh );

#define SS_MESH_EXTENSION "msh"
}

#endif
