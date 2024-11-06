#include "solosnake/testing/testing.hpp"
#include "solosnake/image.hpp"
#include <memory>

// BLUEDATADIR is defined by CMake.

namespace
{
const solosnake::bgra red(0x00, 0x00, 0xFF, 0x00);
const solosnake::bgra green(0x00, 0xFF, 0x00, 0x00);
const solosnake::bgra blue(0xFF, 0x00, 0x00, 0x00);
const solosnake::bgra orange(0x00, 0x80, 0xFF, 0x00);
}

TEST(image, defaults)
{
    solosnake::image img;

    EXPECT_TRUE(img.bytes_size() == 0);
    EXPECT_TRUE(img.width() == 0);
    EXPECT_TRUE(img.height() == 0);
    EXPECT_TRUE(img.format() == solosnake::image::format_bgra);
    EXPECT_TRUE(img == img);
    EXPECT_FALSE(img != img);
}

TEST(image, load_and_widen_bmp)
{
    try
    {
        solosnake::image img(BLUEDATADIR "/textures/testtex256x256.bmp", true);

        EXPECT_TRUE(img.bytes_size() == 256 * 256 * 4);
        EXPECT_TRUE(img.width() == 256);
        EXPECT_TRUE(img.height() == 256);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgra);
        EXPECT_TRUE(img == img);
        EXPECT_FALSE(img != img);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_RGB_bmp)
{
    try
    {
        solosnake::image img(BLUEDATADIR "/textures/testtex256x256rgb.bmp", false);

        EXPECT_TRUE(img.bytes_size() == 256 * 256 * 3);
        EXPECT_TRUE(img.width() == 256);
        EXPECT_TRUE(img.height() == 256);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img == img);
        EXPECT_FALSE(img != img);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, comparison)
{
    try
    {
        solosnake::image img(BLUEDATADIR "/textures/testtex256x256.bmp");

        EXPECT_TRUE(img.bytes_size() == 256 * 256 * 4);
        EXPECT_TRUE(img.width() == 256);
        EXPECT_TRUE(img.height() == 256);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgra);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_1x1_bmp)
{
    try
    {
        solosnake::image imgOriginal(BLUEDATADIR "/textures/test1x1.bmp");
        imgOriginal.save(BLUEDATADIR "/textures/test1x1_2.bmp");

        solosnake::image img(BLUEDATADIR "/textures/test1x1_2.bmp", false);
        EXPECT_TRUE(img.bytes_size() == 3 * 1 * 1);
        EXPECT_TRUE(img.width() == 1);
        EXPECT_TRUE(img.height() == 1);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgr);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_3x1_bmp)
{
    try
    {
        solosnake::image imgOriginal(BLUEDATADIR "/textures/test3x1.bmp");
        imgOriginal.save(BLUEDATADIR "/textures/test3x1_2.bmp");

        solosnake::image img(BLUEDATADIR "/textures/test3x1_2.bmp", false);
        EXPECT_TRUE(img.bytes_size() == 3 * 3 * 1);
        EXPECT_TRUE(img.width() == 3);
        EXPECT_TRUE(img.height() == 1);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgr);

        auto colours = reinterpret_cast<const solosnake::bgr*>(img.data());

        // We expect the colours...

        // Red
        EXPECT_TRUE(colours[0].blue() == 0);
        EXPECT_TRUE(colours[0].green() == 0);
        EXPECT_TRUE(colours[0].red() == 255);
        // Green
        EXPECT_TRUE(colours[1].blue() == 0);
        EXPECT_TRUE(colours[1].green() == 255);
        EXPECT_TRUE(colours[1].red() == 0);
        // Blue
        EXPECT_TRUE(colours[2].blue() == 255);
        EXPECT_TRUE(colours[2].green() == 0);
        EXPECT_TRUE(colours[2].red() == 0);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_3x2_colours)
{
    try
    {
        solosnake::image img(BLUEDATADIR "/textures/test3x2.bmp");

        EXPECT_TRUE(img.bytes_size() == 3 * 2 * 3);
        EXPECT_TRUE(img.width() == 3);
        EXPECT_TRUE(img.height() == 2);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgr);

        auto colours = reinterpret_cast<const solosnake::bgr*>(img.data());

        // We expect the colours...

        // Black
        EXPECT_TRUE(colours[0].blue() == 0);
        EXPECT_TRUE(colours[0].green() == 0);
        EXPECT_TRUE(colours[0].red() == 0);

        // White
        EXPECT_TRUE(colours[1].blue() == 0xFF);
        EXPECT_TRUE(colours[1].green() == 0xFF);
        EXPECT_TRUE(colours[1].red() == 0xFF);

        // Blue
        EXPECT_TRUE(colours[2].blue() == 0xFF);
        EXPECT_TRUE(colours[2].green() == 0);
        EXPECT_TRUE(colours[2].red() == 0);

        // Green
        EXPECT_TRUE(colours[3].blue() == 0);
        EXPECT_TRUE(colours[3].green() == 0xFF);
        EXPECT_TRUE(colours[3].red() == 0);

        // Red
        EXPECT_TRUE(colours[4].blue() == 0);
        EXPECT_TRUE(colours[4].green() == 0);
        EXPECT_TRUE(colours[4].red() == 0xFF);

        // Yellow
        EXPECT_TRUE(colours[5].blue() == 0);
        EXPECT_TRUE(colours[5].green() == 0xFF);
        EXPECT_TRUE(colours[5].red() == 0xFF);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_3x2_colours_widened)
{
    try
    {
        solosnake::image img(BLUEDATADIR "/textures/test3x2.bmp", true);

        EXPECT_TRUE(img.bytes_size() == 3 * 2 * 4);
        EXPECT_TRUE(img.width() == 3);
        EXPECT_TRUE(img.height() == 2);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgra);

        auto colours = reinterpret_cast<const solosnake::bgra*>(img.data());

        // We expect the colours...

        // Black
        EXPECT_TRUE(colours[0].blue() == 0);
        EXPECT_TRUE(colours[0].green() == 0);
        EXPECT_TRUE(colours[0].red() == 0);

        // White
        EXPECT_TRUE(colours[1].blue() == 0xFF);
        EXPECT_TRUE(colours[1].green() == 0xFF);
        EXPECT_TRUE(colours[1].red() == 0xFF);

        // Blue
        EXPECT_TRUE(colours[2].blue() == 0xFF);
        EXPECT_TRUE(colours[2].green() == 0);
        EXPECT_TRUE(colours[2].red() == 0);

        // Green
        EXPECT_TRUE(colours[3].blue() == 0);
        EXPECT_TRUE(colours[3].green() == 0xFF);
        EXPECT_TRUE(colours[3].red() == 0);

        // Red
        EXPECT_TRUE(colours[4].blue() == 0);
        EXPECT_TRUE(colours[4].green() == 0);
        EXPECT_TRUE(colours[4].red() == 0xFF);

        // Yellow
        EXPECT_TRUE(colours[5].blue() == 0);
        EXPECT_TRUE(colours[5].green() == 0xFF);
        EXPECT_TRUE(colours[5].red() == 0xFF);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_3x2_colours_widened_and_saved)
{
    try
    {
        solosnake::image original(BLUEDATADIR "/textures/test3x2.bmp", true);
        original.save(BLUEDATADIR "/textures/test3x2_widened.bmp");

        // Load previously saved image, which should be a BGRA, and see its the
        // same.
        solosnake::image img(BLUEDATADIR "/textures/test3x2_widened.bmp");

        EXPECT_TRUE(img.bytes_size() == 3 * 2 * 4);
        EXPECT_TRUE(img.width() == 3);
        EXPECT_TRUE(img.height() == 2);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgra);

        auto colours = reinterpret_cast<const solosnake::bgra*>(img.data());

        // We expect the colours...

        // Black
        EXPECT_TRUE(colours[0].blue() == 0);
        EXPECT_TRUE(colours[0].green() == 0);
        EXPECT_TRUE(colours[0].red() == 0);

        // White
        EXPECT_TRUE(colours[1].blue() == 0xFF);
        EXPECT_TRUE(colours[1].green() == 0xFF);
        EXPECT_TRUE(colours[1].red() == 0xFF);

        // Blue
        EXPECT_TRUE(colours[2].blue() == 0xFF);
        EXPECT_TRUE(colours[2].green() == 0);
        EXPECT_TRUE(colours[2].red() == 0);

        // Green
        EXPECT_TRUE(colours[3].blue() == 0);
        EXPECT_TRUE(colours[3].green() == 0xFF);
        EXPECT_TRUE(colours[3].red() == 0);

        // Red
        EXPECT_TRUE(colours[4].blue() == 0);
        EXPECT_TRUE(colours[4].green() == 0);
        EXPECT_TRUE(colours[4].red() == 0xFF);

        // Yellow
        EXPECT_TRUE(colours[5].blue() == 0);
        EXPECT_TRUE(colours[5].green() == 0xFF);
        EXPECT_TRUE(colours[5].red() == 0xFF);

        EXPECT_TRUE(img == original);
        EXPECT_FALSE(img != original);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, load_3x2_colours_reloaded)
{
    try
    {
        solosnake::image imgOriginal(BLUEDATADIR "/textures/test3x2.bmp");
        imgOriginal.save(BLUEDATADIR "/textures/test3x2_2.bmp");

        solosnake::image img(BLUEDATADIR "/textures/test3x2_2.bmp", true);
        EXPECT_TRUE(img.bytes_size() == 4 * 2 * 3);
        EXPECT_TRUE(img.width() == 3);
        EXPECT_TRUE(img.height() == 2);
        EXPECT_TRUE(img.format() == solosnake::image::format_bgra);

        auto colours = reinterpret_cast<const solosnake::bgra*>(img.data());

        // We expect the colours...

        // Black
        EXPECT_TRUE(colours[0].blue() == 0);
        EXPECT_TRUE(colours[0].green() == 0);
        EXPECT_TRUE(colours[0].red() == 0);

        // White
        EXPECT_TRUE(colours[1].blue() == 0xFF);
        EXPECT_TRUE(colours[1].green() == 0xFF);
        EXPECT_TRUE(colours[1].red() == 0xFF);

        // Blue
        EXPECT_TRUE(colours[2].blue() == 0xFF);
        EXPECT_TRUE(colours[2].green() == 0);
        EXPECT_TRUE(colours[2].red() == 0);

        // Green
        EXPECT_TRUE(colours[3].blue() == 0);
        EXPECT_TRUE(colours[3].green() == 0xFF);
        EXPECT_TRUE(colours[3].red() == 0);

        // Red
        EXPECT_TRUE(colours[4].blue() == 0);
        EXPECT_TRUE(colours[4].green() == 0);
        EXPECT_TRUE(colours[4].red() == 0xFF);

        // Yellow
        EXPECT_TRUE(colours[5].blue() == 0);
        EXPECT_TRUE(colours[5].green() == 0xFF);
        EXPECT_TRUE(colours[5].red() == 0xFF);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, rgb_bitmap_load_and_save)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/meshes/testtex256x256_2.bmp"));

        // Loaded image again, no widening.
        solosnake::image img2(BLUEDATADIR "/meshes/testtex256x256_2.bmp", false);
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgr);

        // We expect to load exact same image we saved:
        EXPECT_TRUE(img2 == img1);

        // Test operator !=
        EXPECT_FALSE(img2 != img1);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, rgb_bitmap_default_load_is_no_widen)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp"));

        // Loaded image again, implicit no widening.
        solosnake::image img2(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp");
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgr);

        // We expect to load exact same image we saved:
        EXPECT_TRUE(img2 == img1);

        // Test operator !=
        EXPECT_FALSE(img2 != img1);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, rgb_bitmap_load_widened)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp"));

        // Loaded image again, explicit widening
        solosnake::image img2(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp", true);
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        // We expect to load a different image now to the one we saved.
        EXPECT_TRUE(img2 != img1);

        // Test operator !=
        EXPECT_FALSE(img2 == img1);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, save_bmp)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp"));

        // Loaded image is same image but now has alpha channel due to widening.
        solosnake::image img2(BLUEDATADIR "/meshes/testtex256x256rgb_2.bmp", true);
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        EXPECT_TRUE(img2.save(BLUEDATADIR "/meshes/testtex256x256_3.bmp"));
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        solosnake::image img3(BLUEDATADIR "/meshes/testtex256x256_3.bmp");

        // Test operator ==
        EXPECT_TRUE(img2 == img3);

        // Test operator !=
        EXPECT_FALSE(img3 != img2);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, widen_added_alpha_channel)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/textures/testtex256x256rgb_2.bmp"));

        // Loaded image is same image but now has alpha channel.
        solosnake::image img2(BLUEDATADIR "/textures/testtex256x256rgb_2.bmp", true);
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        // Load image again
        solosnake::image img3(BLUEDATADIR "/textures/testtex256x256rgb_2.bmp", true);
        EXPECT_TRUE(img3.format() == solosnake::image::format_bgra);
        EXPECT_TRUE(img3 == img2);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, widened_added_loads_and_saves)
{
    try
    {
        // Load image as widened
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp", true);
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgra);

        // Save and reload.
        img1.save(BLUEDATADIR "/textures/testtex256x256rgb_wide.bmp");

        solosnake::image img2(BLUEDATADIR "/textures/testtex256x256rgb_wide.bmp");
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        EXPECT_TRUE(img2 == img1);

        // Check loading widened wide image has no effect.
        solosnake::image img3(BLUEDATADIR "/textures/testtex256x256rgb_wide.bmp", true);

        EXPECT_TRUE(img3 == img1);
        EXPECT_TRUE(img3 == img2);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, save_bmp_with_alpha)
{
    try
    {
        // Load bgr and save to create a bgra...
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        EXPECT_TRUE(img1.format() == solosnake::image::format_bgr);
        EXPECT_TRUE(img1.save(BLUEDATADIR "/textures/testtex256x256rgb_2.bmp"));

        // Loaded image is same image but now has alpha channel due to widening.
        solosnake::image img2(BLUEDATADIR "/textures/testtex256x256rgb_2.bmp", true);
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        // Copy red channel into alpha channel:
        for (size_t i = 0; i < img2.width(); ++i)
        {
            for (size_t j = 0; j < img2.height(); ++j)
            {
                unsigned char* pixelBGRA
                    = solosnake::image::pixel(i, j, img2, solosnake::image::format_bgra);
                pixelBGRA[3] = pixelBGRA[0];
            }
        }

        // Save img2 with new alpha data to new image file (3).
        EXPECT_TRUE(img2.save(BLUEDATADIR "/textures/testtex256x256rgb_3.bmp"));
        EXPECT_TRUE(img2.format() == solosnake::image::format_bgra);

        // Load new image file.
        solosnake::image img3(BLUEDATADIR "/textures/testtex256x256rgb_3.bmp");

        EXPECT_TRUE(img2 == img3);
        EXPECT_FALSE(img3 != img2);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, compare_layer)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");

        EXPECT_FALSE(img1.empty());

        EXPECT_TRUE(solosnake::image::compare_layer(img1, 0, img1, 0) == 0);
        EXPECT_TRUE(solosnake::image::compare_layer(img1, 1, img1, 1) == 0);
        EXPECT_TRUE(solosnake::image::compare_layer(img1, 1, img1, 1) == 0);

        EXPECT_FALSE(solosnake::image::compare_layer(img1, 1, img1, 0) == 0);
        EXPECT_FALSE(solosnake::image::compare_layer(img1, 1, img1, 2) == 0);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, copy_layer_image_to_image)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");
        solosnake::image img2(img1.width(), img1.height(), img1.format());

        EXPECT_TRUE(img1 != img2);

        // Build a copy, layer by layer:
        for (size_t i = 0; i < img1.layers_count(); ++i)
        {
            solosnake::image::copy_layer(img1, i, img2, i);
        }

        EXPECT_TRUE(img1 == img2);
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, copy_layer_within_image)
{
    try
    {
        solosnake::image img1(BLUEDATADIR "/textures/testtex256x256rgb.bmp");

        for (size_t i = 1; i < img1.layers_count(); ++i)
        {
            EXPECT_TRUE(0 != solosnake::image::compare_layer(img1, 0, img1, i));
        }

        // Copy layer 0 to other layers:
        for (size_t i = 1; i < img1.layers_count(); ++i)
        {
            solosnake::image::copy_layer(img1, 0, img1, i);
        }

        for (size_t i = 1; i < img1.layers_count(); ++i)
        {
            EXPECT_TRUE(0 == solosnake::image::compare_layer(img1, 0, img1, i));
        }
    }
    catch (...)
    {
        ADD_FAILURE("File not found.");
    }
}

TEST(image, rgb_blend)
{
    using namespace solosnake;

    image img(512, 512, image::format_bgr);

    EXPECT_NO_THROW(img.draw_triangle(0.1f, 0.1f, red, 0.1f, 0.9f, orange, 0.9f, 0.9f, green));

    EXPECT_NO_THROW(img.draw_triangle(0.1f, 0.1f, red, 0.9f, 0.1f, blue, 0.9f, 0.9f, green));

    EXPECT_NO_THROW(img.save("rbg_blend.bmp"));
}

TEST(image, rgb_triangle_blend)
{
    using namespace solosnake;

    image img(512, 512, image::format_bgr);

    EXPECT_NO_THROW(img.draw_triangle(0.5f, 0.1f, red, 0.1f, 0.9f, blue, 0.9f, 0.9f, green));

    EXPECT_NO_THROW(img.save("rbg_tri.bmp"));
}
