#include "solosnake/testing/testing.hpp"
#include "solosnake/screenxy.hpp"

using namespace solosnake;

TEST(screenxy, ctor32_zero)
{
    {
        screenxy xy(0);

        EXPECT_TRUE(xy.screen_x() == 0);
        EXPECT_TRUE(xy.screen_y() == 0);
    }
}

TEST(screenxy, ctor16_zeros)
{
    {
        screenxy xy(0, 0);

        EXPECT_TRUE(xy.screen_x() == 0);
        EXPECT_TRUE(xy.screen_y() == 0);
    }
}

TEST(screenxy, ctor32_1_2)
{
    {
        screenxy xy(1, 2);

        EXPECT_TRUE(xy.screen_x() == 1);
        EXPECT_TRUE(xy.screen_y() == 2);
    }
}

TEST(screenxy, ctor32_11_212)
{
    {
        screenxy xy(11, 212);

        EXPECT_TRUE(xy.screen_x() == 11);
        EXPECT_TRUE(xy.screen_y() == 212);
    }
}

TEST(screenxy, ctor16_0xDEAD_0xBEEF)
{
    {
        screenxy xy(short(0xDEAD), short(0xBEEF));

        EXPECT_TRUE(xy.screen_x() == -8531);
        EXPECT_TRUE(xy.screen_y() == -16657);
    }
}

TEST(screenxy, ctor0xDEADBEEF)
{
    {
        screenxy xy(0xBEEFDEAD);

        EXPECT_TRUE(xy.screen_x() == -8531);  // DEAD
        EXPECT_TRUE(xy.screen_y() == -16657); // BEEF
        EXPECT_TRUE(xy.screen_xy() == 0xBEEFDEAD);
    }
}

TEST(screenxy, negatives1)
{
    {
        screenxy xy(-1, -1);

        EXPECT_TRUE(xy.screen_x() == -1);
        EXPECT_TRUE(xy.screen_y() == -1);
    }
}

TEST(screenxy, negatives2)
{
    {
        screenxy xy(-2, -2);

        EXPECT_TRUE(xy.screen_x() == -2);
        EXPECT_TRUE(xy.screen_y() == -2);
    }
}

TEST(screenxy, loop_test)
{
    {
        const short n = 10000;

        // Step through in 7 and 3, to speed up
        // but hit as many as we can.
        for (short i = -n; i < n; i += 7)
        {
            for (short j = -n; j < n; j += 3)
            {
                screenxy xy(i, j);

                EXPECT_TRUE(xy.screen_x() == i);
                EXPECT_TRUE(xy.screen_y() == j);
            }
        }
    }
}
