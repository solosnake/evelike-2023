#include "solosnake/testing/testing.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/point.hpp"
#include <cstdlib>

namespace
{
bool almost_same(float a, float b)
{
    float d = std::fabs(a - b);
    // Hand tested to find a number that passed within reason.
    return d <= 0.002f;
}
}

TEST(matrix3d, mul)
{
    {
        solosnake::matrix4x4_t m1;
        solosnake::matrix4x4_t m2;

        for (size_t i = 0; i < 16; ++i)
        {
            m1[i] = 1.0f + i;
            m2[i] = 1.0f + i;
        }

        solosnake::matrix4x4_t m12;

        solosnake::mul_4x4(m1, m2, m12);

        // Answer from
        // http://www.euclideanspace.com/maths/algebra/matrix/arithmetic/fourD/index.htm

        EXPECT_TRUE(m12[0] == 90.0f);
        EXPECT_TRUE(m12[1] == 100.0f);
        EXPECT_TRUE(m12[2] == 110.0f);
        EXPECT_TRUE(m12[3] == 120.0f);

        EXPECT_TRUE(m12[4] == 202.0f);
        EXPECT_TRUE(m12[5] == 228.0f);
        EXPECT_TRUE(m12[6] == 254.0f);
        EXPECT_TRUE(m12[7] == 280.0f);

        EXPECT_TRUE(m12[8] == 314.0f);
        EXPECT_TRUE(m12[9] == 356.0f);
        EXPECT_TRUE(m12[10] == 398.0f);
        EXPECT_TRUE(m12[11] == 440.0f);

        EXPECT_TRUE(m12[12] == 426.0f);
        EXPECT_TRUE(m12[13] == 484.0f);
        EXPECT_TRUE(m12[14] == 542.0f);
        EXPECT_TRUE(m12[15] == 600.0f);
    }
}

TEST(matrix3d, invert_identity)
{
    {
        solosnake::matrix4x4_t m1;
        solosnake::load_identity_4x4(m1);

        solosnake::matrix4x4_t m2 = { 0.0f };
        solosnake::invert_4x4(m1, m2);

        EXPECT_TRUE(solosnake::identifical_4x4(m1, m2));
    }
}

TEST(matrix3d, invertion)
{
    {
        solosnake::matrix4x4_t m1
            = { 3.000f, 0.000f, -3.000f, 0.000f, 1.000f, 2.000f, 4.000f, 1.000f,
                2.000f, 1.000f, 4.000f,  0.000f, 1.000f, 2.000f, 3.000f, 4.000f, };

        solosnake::matrix4x4_t inv;

        solosnake::matrix4x4_t invExpected
            = { 0.183f,  -0.200f, 0.300f, 0.050f, 0.233f,  1.200f,  -0.800f, -0.300f,
                -0.150f, -0.200f, 0.300f, 0.050f, -0.050f, -0.400f, 0.100f,  0.350f, };

        solosnake::invert_4x4(m1, inv);

        for (size_t i = 0; i < 16; ++i)
        {
            EXPECT_TRUE(std::abs(invExpected[i] - inv[i]) < 0.01);
        }
    }
}

TEST(matrix3d, determinant)
{
    {
        solosnake::matrix4x4_t m;
        solosnake::load_xyz_ry_4x4(2, 3, 4, solosnake::radians(PI_RADIANS * 0.0f), m);
        EXPECT_TRUE(almost_same(solosnake::rotation_determinant_4x4(m), 1.0f));
    }
}

TEST(matrix3d, projection_theory)
{
    {
        solosnake::matrix4x4_t proj, view;
        solosnake::load_perspective(
            solosnake::radians(PI_RADIANS * 0.25f), float(800) / 600, 0.1f, 100.0f, proj);
        solosnake::load_xyz_ry_4x4(2, 3, 4, solosnake::radians(PI_RADIANS * 0.0f), view);

        solosnake::matrix4x4_t vp;
        solosnake::mul_4x4(view, proj, vp);

        solosnake::matrix4x4_t vpInv;
        EXPECT_TRUE(solosnake::invert_4x4(vp, vpInv));

        float pos[] = { 0, 0, 50, 1.0f };
        float posVP[4];
        float pos2[4];

        // Transform point to view-space:
        solosnake::transform_4x1(pos, vp, posVP);

        // Homogenise
        solosnake::homogenise(posVP);

        // Transform back to a new pos in world space?
        solosnake::transform_4x1(posVP, vpInv, pos2);

        solosnake::homogenise(pos2);

        EXPECT_TRUE(almost_same(pos[0], pos2[0]));
        EXPECT_TRUE(almost_same(pos[1], pos2[1]));
        EXPECT_TRUE(almost_same(pos[2], pos2[2]));
        EXPECT_TRUE(almost_same(pos[3], pos2[3]));
    }
}