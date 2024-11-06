#include "solosnake/testing/testing.hpp"
#include "solosnake/skybox.hpp"

TEST( skybox, save_skybox )
{
    try
    {
        auto debugging = true;
        auto xp  = std::string("./test-assets/skyboxes/seascape/right.png");
        auto xn  = std::string("./test-assets/skyboxes/seascape/left.png");
        auto yp  = std::string("./test-assets/skyboxes/seascape/top.png");
        auto yn  = std::string("./test-assets/skyboxes/seascape/bottom.png");
        auto zp  = std::string("./test-assets/skyboxes/seascape/front.png");
        auto zn  = std::string("./test-assets/skyboxes/seascape/back.png");
        solosnake::save_skybox("SkyboxTest.sky", { xp, xn, yp, yn, zp, zn });
    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}