#include "solosnake/testing/testing.hpp"
#include "solosnake/image.hpp"
#include "solosnake/tga.hpp"

using namespace solosnake;

TEST(PNG, PNG)
{
    Image img("test-assets/rgb5x7.png");
    img.save("red.bmp");
}

TEST(PNG, PNGALPHA)
{
    Image img("test-assets/rgb5x7alpha.png");
    img.save("reda1.bmp");
    EXPECT_TRUE( img.format() == Image::Format::BGRA );
}

TEST(PNG, PNG_REMOVE_ALPHA)
{
    Image img_no_a("test-assets/rgb5x7alpha.png", Image::Format::BGR);
    EXPECT_TRUE( img_no_a.format() == Image::Format::BGR );
    img_no_a.save("reda2.bmp");
}

TEST(PNG, HELMET)
{
    Image helmet("test-assets/SciFiHelmet_BaseColor.png", Image::Format::BGR);
    helmet.save("helmet.bmp");
    save_bgr_tga(helmet.width(), helmet.height(), helmet.pixels(), false, "helmet.tga");
}
