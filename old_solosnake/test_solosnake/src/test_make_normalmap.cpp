//#define RUN_TESTS
#ifdef RUN_TESTS

#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/load_models.hpp"
#include "solosnake/save_models.hpp"
#include "solosnake/make_normalmap.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/image.hpp"

using namespace solosnake;

TEST(make_normalmap, Soyuz)
{
    try
    {
        solosnake::datadirectory assetsDir("./meshes");

        const filepath url = assetsDir.get_file("soyuz.lua");

        auto meshes = load_mesh_datas(url);
        auto texcoords = load_texcoords(url);

        EXPECT_TRUE(texcoords.size() == 18);
        EXPECT_TRUE(meshes.size() == 18);

        for (size_t i = 0; i < meshes.size(); ++i)
        {
            unsigned int w = 1024;
            unsigned int h = 1024;

            solosnake::image flat
                = make_normalmap(*meshes.at(i), *texcoords.at(i), w, h, false, 255);
            solosnake::image smooth
                = make_normalmap(*meshes.at(i), *texcoords.at(i), w, h, true, 255);
            std::ostringstream os;

            os << "./meshes/soyuz." << i << ".flat..bmp";
            EXPECT_TRUE(flat.save(os.str().c_str()));

            os.str(std::string());

            os << "./meshes/soyuz." << i << ".smooth..bmp";
            EXPECT_TRUE(smooth.save(os.str().c_str()));
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

TEST(make_normalmap, cube)
{
    try
    {
        solosnake::datadirectory assetsDir("./meshes");

        const filepath url = assetsDir.get_file("thruster.lua");

        auto meshes = load_mesh_datas(url);
        auto texcoords = load_texcoords(url);

        EXPECT_TRUE(texcoords.size() == 1);
        EXPECT_TRUE(meshes.size() == 1);

        for (size_t i = 0; i < meshes.size(); ++i)
        {
            unsigned int w = 512;
            unsigned int h = 1024;

            solosnake::image flat
                = make_normalmap(*meshes.at(i), *texcoords.at(i), w, h, false, 255);
            solosnake::image smooth
                = make_normalmap(*meshes.at(i), *texcoords.at(i), w, h, true, 255);
            std::ostringstream os;

            os << "./meshes/cube." << i << ".flat.bmp";
            EXPECT_TRUE(flat.save(os.str().c_str()));

            os.str(std::string());

            os << "./meshes/cube." << i << ".smooth.bmp";
            EXPECT_TRUE(smooth.save(os.str().c_str()));
        }
    }
    catch (...)
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

#endif
