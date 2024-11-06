#include "solosnake/testing/testing.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/bgra.hpp"

using namespace solosnake;

TEST(colour, fromtext)
{
    bgra myColour("0xFF00AA42");
    EXPECT_TRUE(myColour.blue() == 0xFF);
    EXPECT_TRUE(myColour.green() == 0x00);
    EXPECT_TRUE(myColour.red() == 0xAA);
    EXPECT_TRUE(myColour.alpha() == 0x42);

    bgra red("0x0000FFFF");
    bgra red1("65535");
    EXPECT_TRUE(red1 == red);
}

TEST(colour, equality)
{
    bgra c1("0xFF00AA42");
    bgra c2("0xFF00AA42");
    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 != c2);
}

TEST(colour, ctor4)
{
    bgra c1(1, 22, 33, 44);
    EXPECT_TRUE(c1.blue() == 1);
    EXPECT_TRUE(c1.green() == 22);
    EXPECT_TRUE(c1.red() == 33);
    EXPECT_TRUE(c1.alpha() == 44);
}

TEST(colour, ctor1)
{
    bgra c1(8);
    EXPECT_TRUE(c1.blue() == 8);
    EXPECT_TRUE(c1.green() == 8);
    EXPECT_TRUE(c1.red() == 8);
    EXPECT_TRUE(c1.alpha() == 8);
}
