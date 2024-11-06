#include "solosnake/testing/testing.hpp"
#include "solosnake/evelike-show-model.hpp"
#include "solosnake/gltf-to-mesh.hpp"
#include <iostream>

TEST( show_model, show_model )
{
    try
    {
        std::cout << "In test show model\n";

        auto debugging = true;
        auto src  = "./test-assets/SciFiHelmet.gltf";
        auto msh = "ShowModelTest.msh";
        solosnake::convert_glTF_to_mesh(src, msh);
        auto result = solosnake::evelike::show_model(msh, "", debugging, 12000);
    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}

TEST( show_model, show_model_with_skybox )
{
    try
    {
        auto debugging = true;
        auto src = "./test-assets/SciFiHelmet.gltf";
        auto box = "./test-assets/Skybox.sky";
        auto msh = "ShowModelTest.msh";
        solosnake::convert_glTF_to_mesh(src, msh);
        auto result = solosnake::evelike::show_model(msh, box, debugging, 0);
    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}

TEST( show_model, no_model )
{
    try
    {
        auto debugging = true;
        auto result = solosnake::evelike::show_model("", "", debugging, 1200);
    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}