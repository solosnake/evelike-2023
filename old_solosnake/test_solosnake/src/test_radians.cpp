#include "solosnake/testing/testing.hpp"
#include "solosnake/radians.hpp"

using namespace solosnake;

TEST(radians, clamped)
{
    EXPECT_EQ(0.0f, TWOPI_RADIANS.clamped().value());
    EXPECT_EQ(0.0f, radians(-TWOPI_RADIANS).clamped().value());
    EXPECT_EQ(6.0f, radians(+6.0f).clamped().value());
    EXPECT_TRUE(radians(-1.0f + TWOPI_RADIANS.value()) == radians(-1.0f).clamped());
}