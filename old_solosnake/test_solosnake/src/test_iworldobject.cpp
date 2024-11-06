#include "solosnake/testing/testing.hpp"
#include "solosnake/iworldobject.hpp"
#include <memory>

using namespace solosnake;

TEST(iworldobject, default)
{
    iworldobject w;

    EXPECT_TRUE(w.is_valid());
    EXPECT_TRUE(solosnake::point3d(0.0f, 1.0f, 0.0f) == w.up());
    EXPECT_TRUE(solosnake::point3d(0.0f, 0.0f, 1.0f) == w.forward());
    EXPECT_TRUE(solosnake::point3d(1.0f, 0.0f, 0.0f) == w.left());
    EXPECT_TRUE(solosnake::point3d(0.0f, 0.0f, 0.0f) == w.dirvector());
    EXPECT_TRUE(solosnake::point3d(0.0f, 0.0f, 0.0f) == w.velocity());
    EXPECT_TRUE(0.0f == w.speed());
}

TEST(iworldobject, validity)
{
    iworldobject w;

    EXPECT_TRUE(w.is_valid());

    w.make_invalid();

    EXPECT_FALSE(w.is_valid());
}