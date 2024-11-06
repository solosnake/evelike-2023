#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardpoint_shooter.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Hardpoint_shooter, to_from_json )
{
    Hardpoint_shot  hardpoint_shot{ CapDrainHardpoint, 1u, 23u, {} };
    Fixed_angle    rotation_speed{180};
    std::uint16_t   firing_cap_cost{12u};
    std::uint16_t   firing_cooldown{3u};
    std::int16_t    min_angle_degrees{10};
    std::int16_t    max_angle_degrees{32};
    std::uint16_t   min_range{15u};
    std::uint16_t   max_range_percent_effect{55u};
    std::uint8_t    reliability{80u};

    Hardpoint_shooter h( hardpoint_shot,
                         rotation_speed,
                         firing_cap_cost,
                         firing_cooldown,
                         min_angle_degrees,
                         max_angle_degrees,
                         min_range,
                         max_range_percent_effect,
                         reliability );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = h;
    auto h2 = j.get<Hardpoint_shooter>();
    EXPECT_EQ( h, h2 );
}

TEST( Hardpoint_shooter, falloff )
{
    // calculate_falloff: target_range, min_range, max_range, max_range_effect

    // max_range_percent_effect : 0
    // ----------------------------
    // Target at min range: expect no falloff e.g. 1.0f
    EXPECT_EQ( 1.0f, Hardpoint_shooter::calculate_falloff( 10, 10, 20, 0 ) );
    // Target at max range: expect total falloff e.g. 0.0f
    EXPECT_EQ( 0.0f, Hardpoint_shooter::calculate_falloff( 20, 10, 20, 0 ) );
    // Target at half range: expect falloff of 0.5f.
    EXPECT_EQ( 0.5, Hardpoint_shooter::calculate_falloff( 15, 10, 20, 0 ) );

    // max_range_percent_effect : 100
    // ------------------------------
    // Expect this weapon to be as effective at max range as at min:
    // Target at min range: expect no falloff e.g. 1.0f
    EXPECT_EQ( 1.0f, Hardpoint_shooter::calculate_falloff( 10, 10, 20, 100 ) );
    // Target at max range: expect still no falloff e.g. 1.0f
    EXPECT_EQ( 1.0f, Hardpoint_shooter::calculate_falloff( 20, 10, 20, 100 ) );
    // Target at half range: expect still no falloff e.g. 1.0f
    EXPECT_EQ( 1.0, Hardpoint_shooter::calculate_falloff( 15, 10, 20, 100 ) );

    // max_range_percent_effect : 50
    // ------------------------------
    // Expect this weapon to be half as effective at max range as at min:
    // Target at min range: expect no falloff e.g. 1.0f
    EXPECT_EQ( 1.0f, Hardpoint_shooter::calculate_falloff( 10, 10, 20, 50 ) );
    // Target at max range: expect falloff 0.5f
    EXPECT_EQ( 0.5f, Hardpoint_shooter::calculate_falloff( 20, 10, 20, 50 ) );
    // Target at half range: expect falloff 0.75f (falloff at max range is 0.5)
    EXPECT_EQ( 0.75f, Hardpoint_shooter::calculate_falloff( 15, 10, 20, 50 ) );

    // max_range_percent_effect : 200
    // ------------------------------
    // Expect this weapon to be twice as effective at max range as at min:
    // Target at min range: expect no falloff e.g. 1.0f
    EXPECT_EQ( 1.0f, Hardpoint_shooter::calculate_falloff( 10, 10, 20, 200 ) );
    // Target at max range: expect 'falloff' of 2.0f
    EXPECT_EQ( 2.0f, Hardpoint_shooter::calculate_falloff( 20, 10, 20, 200 ) );
    // Target at half range: expect 'falloff' of 1.5f
    EXPECT_EQ( 1.5, Hardpoint_shooter::calculate_falloff( 15, 10, 20, 200 ) );
}
TEST( Hardpoint_shooter, get_mid_angle )
{
    Hardpoint_shot  hardpoint_shot{ CapDrainHardpoint, 1u, 23u, {} };
    Fixed_angle    rotation_speed{180};
    std::uint16_t   firing_cap_cost{12u};
    std::uint16_t   firing_cooldown{3u};
    std::int16_t    min_angle_degrees{0};
    std::int16_t    max_angle_degrees{180};
    std::uint16_t   min_range{15u};
    std::uint16_t   max_range_percent_effect{55u};
    std::uint8_t    reliability{80u};

    Hardpoint_shooter h( hardpoint_shot,
                         rotation_speed,
                         firing_cap_cost,
                         firing_cooldown,
                         min_angle_degrees,
                         max_angle_degrees,
                         min_range,
                         max_range_percent_effect,
                         reliability );

    auto left  = h.min_angle();
    auto right = h.max_angle();
    EXPECT_EQ( h.get_mid_angle(), (right - left)/2);
}