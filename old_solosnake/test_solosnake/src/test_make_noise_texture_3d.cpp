#define RUN_TESTS
#ifdef RUN_TESTS

#include <sstream>
#include <cmath>
#include "solosnake/testing/testing.hpp"
#include "solosnake/make_noise_texture_3d.hpp"
#include "solosnake/image.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/clamp.hpp"
#include "solosnake/mix.hpp"

using namespace solosnake;

TEST(make_noise_texture_3d, make_noise_texture_3d)
{
    std::vector<std::uint8_t> t = make_noise_texture_3d(power_of_2(7u), 4u);
    auto ptr = t.data();

    unsigned int texels_count = static_cast<unsigned int>(t.size() / 4u);
    unsigned int side_length = 128;

    EXPECT_EQ(128 * 128 * 128, texels_count);

    image n(side_length, side_length, image::format_bgra);

    colour Color1(0.8f, 0.7f, 0.0f, 0.0f);
    colour Color2(0.6f, 0.1f, 0.0f, 0.0f);

    for (unsigned int y = 0u; y < side_length; ++y)
    {
        for (unsigned int x = 0u; x < side_length; ++x)
        {
            auto p = ptr + (4u * (x + (y * side_length)));

            double noisevec[4] = {(p[0] / 255.0), (p[1] / 255.0), (p[2] / 255.0), (p[3] / 255.0) };

            double intensity = abs(noisevec[0] - 0.25) + abs(noisevec[1] - 0.125)
                               + abs(noisevec[2] - 0.0625) + abs(noisevec[3] - 0.03125);

            intensity = sin((y / 128.0) * 6.0 + intensity * 12.0) * 0.5 + 0.5;

            colour c = mix(Color1, Color2, intensity);

            n.set_pixel(x, y, bgra(c));
        }
    }

    n.save("noisey.bmp");
}

#endif
