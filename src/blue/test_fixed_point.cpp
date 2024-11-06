#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"

using namespace blue;

TEST( angle_as_radians, angle_as_radians )
{
    EXPECT_FLOAT_EQ( (0.5f * BLUE_PI), fixed_angle_to_radians(BLUE_INT32_HALF_PI) );
}
