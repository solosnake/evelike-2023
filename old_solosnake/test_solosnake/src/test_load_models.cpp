#define RUN_TESTS
#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include <fstream>
#include "solosnake/load_meshes.hpp"
#include "solosnake/save_models.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/image.hpp"

using namespace solosnake;
using namespace std;

TEST(meshdata, create_save_and_reload_meshdata)
{
    try
    {
        meshdata d;

        image pic(64, 64, image::format_bgra);
        pic.save("diffAndSpecName.bmp");
        pic.save("normalsAndEmissName.bmp");

        d.diffAndSpecName = "diffAndSpecName.bmp";
        d.normalsAndEmissName = "normalsAndEmissName.bmp";

        d.indices.push_back(0u);
        d.indices.push_back(1u);
        d.indices.push_back(2u);

        d.uvs.push_back(0.0f);
        d.uvs.push_back(1.0f);
        d.uvs.push_back(1.0f);
        d.uvs.push_back(1.0f);
        d.uvs.push_back(0.0f);
        d.uvs.push_back(0.0f);

        d.vertices.push_back(0.5f);
        d.vertices.push_back(0.5f);
        d.vertices.push_back(0.5f);
        d.vertices.push_back(-0.5f);
        d.vertices.push_back(-0.5f);
        d.vertices.push_back(-0.5f);
        d.vertices.push_back(0.0f);
        d.vertices.push_back(1.5f);
        d.vertices.push_back(1.5f);

        d.tangents.push_back(0.0f);
        d.tangents.push_back(1.0f);
        d.tangents.push_back(0.0f);
        d.tangents.push_back(0.0f);
        d.tangents.push_back(1.0f);
        d.tangents.push_back(0.0f);
        d.tangents.push_back(0.0f);
        d.tangents.push_back(1.0f);
        d.tangents.push_back(0.0f);

        d.smoothednormals.push_back(0.0f);
        d.smoothednormals.push_back(1.0f);
        d.smoothednormals.push_back(0.0f);
        d.smoothednormals.push_back(0.0f);
        d.smoothednormals.push_back(1.0f);
        d.smoothednormals.push_back(0.0f);
        d.smoothednormals.push_back(0.0f);
        d.smoothednormals.push_back(1.0f);
        d.smoothednormals.push_back(0.0f);

        std::vector<meshdata> meshes;

        d.name = "MESH1";
        meshes.push_back(d);

        d.name = "MESH2";
        meshes.push_back(d);

        d.name = "MESH3";
        meshes.push_back(d);

        save_mesh_datas("test.msh", meshes);

        auto thisDir = make_shared<datadirectory>(".");
        auto imeshes = load_mesh_datas("test.msh", thisDir);

        EXPECT_TRUE(meshes.size() == imeshes.size());
        EXPECT_TRUE(meshes[0].name == imeshes[0]->get_mesh_name());
        EXPECT_TRUE(meshes[1].name == imeshes[1]->get_mesh_name());
        EXPECT_TRUE(meshes[2].name == imeshes[2]->get_mesh_name());
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

#endif
