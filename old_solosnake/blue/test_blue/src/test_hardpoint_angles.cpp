#define RUN_TESTS
#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardpoint_angles.hpp"

using namespace blue;

TEST( relative_angle_to_target, location_and_target_same )
{
    FixedAngle_t facingAngle = 900000;
    hexcoord location = hexcoord::make_coord( 1, 4 );
    hexcoord targetLocation = hexcoord::make_coord( 1, 4 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, bug_18_10_2015 )
{
    // These coords cause an assert:
    EXPECT_TRUE( BLUE_FACING_3 == 900000 );
    FixedAngle_t facingAngle = 900000;
    hexcoord location = hexcoord::make_coord( 1, 4 );
    hexcoord targetLocation = hexcoord::make_coord( 2, 4 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 2400000 );
}

TEST( relative_angle_to_target, facing_target_0 )
{
    FixedAngle_t facingAngle = BLUE_FACING_0;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 1, 0 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, facing_target_1 )
{
    FixedAngle_t facingAngle = BLUE_FACING_1;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 2, 1 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, facing_target_2 )
{
    FixedAngle_t facingAngle = BLUE_FACING_2;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 2, 2 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, facing_target_3 )
{
    FixedAngle_t facingAngle = BLUE_FACING_3;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 1, 2 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, facing_target_4 )
{
    FixedAngle_t facingAngle = BLUE_FACING_4;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 0, 2 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}

TEST( relative_angle_to_target, facing_target_5 )
{
    FixedAngle_t facingAngle = BLUE_FACING_5;
    hexcoord location = hexcoord::make_coord( 1, 1 );
    hexcoord targetLocation = hexcoord::make_coord( 0, 1 );
    FixedAngle_t a = relative_angle_to_target( facingAngle, location, targetLocation );
    EXPECT_TRUE( a == 0 );
}
#endif
