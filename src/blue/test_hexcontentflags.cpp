#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"

TEST( HEX_FLAGS, offboard_tile_value )
{
    EXPECT_EQ( blue::off_board_tile_value(), blue::HEX_FLAGS_OFF_BOARD );
}

TEST( HEX_FLAGS, sun_strength_mask )
{
    EXPECT_EQ( blue::sun_strength_mask(), blue::HEX_FLAGS_SUN_STRENGTH);
}

TEST( HEX_FLAGS, asteroid_tile_value )
{
    EXPECT_EQ( blue::asteroid_tile_value(), blue::HEX_FLAGS_ASTEROID );
}

TEST( HEX_FLAGS, bot_tile_value )
{
    EXPECT_EQ( blue::bot_tile_value(), blue::HEX_FLAGS_BOT );
}

TEST( HEX_FLAGS, reserved_tile_value )
{
    EXPECT_EQ( blue::reserved_tile_value(), blue::HEX_FLAGS_RESERVED );
}