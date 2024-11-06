#include "solosnake/testing/testing.hpp"
#include "solosnake/mesh_cache.hpp"
#include "solosnake/datadirectory.hpp"

using namespace solosnake;

// BLUEDATADIR is defined by CMake.

TEST(mesh_cache, load_mesh_file)
{
    try
    {
        auto meshesDir = std::make_shared<datadirectory>(BLUEDATADIR "/meshes");
        auto texesDir = std::make_shared<datadirectory>(BLUEDATADIR "/textures");

        mesh_cache mc(meshesDir, texesDir);

        EXPECT_TRUE(mc.size() == 0);
        mc.load_meshes_file("thruster.msh");
        EXPECT_TRUE(mc.size() > 0);
    }
    catch (...)
    {
        ADD_FAILURE("Exception.");
    }
}

TEST(mesh_cache, load_imesh)
{
    try
    {
        auto meshesDir = std::make_shared<datadirectory>(BLUEDATADIR "/meshes");
        auto texesDir = std::make_shared<datadirectory>(BLUEDATADIR "/textures");

        mesh_cache mc(meshesDir, texesDir);
        EXPECT_TRUE(mc.size() == 0);
        mc.load_meshes_file("thruster.msh");
        EXPECT_TRUE(mc.size() > 0);

        auto m = mc.get_imesh("thruster00");
        EXPECT_TRUE(m != nullptr);
        EXPECT_TRUE(m->get_mesh_name() == "thruster00");
    }
    catch (...)
    {
        ADD_FAILURE("Exception.");
    }
}
