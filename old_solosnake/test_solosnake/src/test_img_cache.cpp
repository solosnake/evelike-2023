#include "solosnake/testing/testing.hpp"
#include "solosnake/img_cache.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer_types.hpp"

using namespace solosnake;

TEST(img_cache, load_texture)
{
    try
    {
        auto imgDir = std::make_shared<datadirectory>(BLUEDATADIR "/textures");

        img_cache ic(imgDir);
        EXPECT_TRUE(ic.size() == 0);

        auto picsrc = ic.get_img_src("testtex256x256.bmp");
        EXPECT_TRUE(0 == picsrc->get_image_name().compare("testtex256x256.bmp"));

        auto pic = picsrc->get_image();
        EXPECT_TRUE(pic != nullptr);
        EXPECT_TRUE(pic->img_width() > 0);
        EXPECT_TRUE(ic.size() == 1);
    }
    catch (...)
    {
        ADD_FAILURE("Exception caught.");
    }
}
