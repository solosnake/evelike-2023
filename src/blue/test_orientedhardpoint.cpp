#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_oriented_hardpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Oriented_hardpoint, Oriented_hardpoint )
{
    const Hardpoint_shot hardpointshot{ CapDrainHardpoint, 1u, 23u, {} };
    const Fixed_angle  rotation_speed{180};
    const std::uint16_t firing_cap_cost{12u};
    const std::uint16_t firing_cooldown{3u};
    const std::int16_t  min_angle_degrees{0};
    const std::int16_t  max_angle_degrees{180};
    const std::uint16_t min_range{15u};
    const std::uint16_t max_range_percent_effect{55u};
    const std::uint8_t  reliability{80u};
    const Hardpoint_shooter shooter( hardpointshot,
                                    rotation_speed,
                                    firing_cap_cost,
                                    firing_cooldown,
                                    min_angle_degrees,
                                    max_angle_degrees,
                                    min_range,
                                    max_range_percent_effect,
                                    reliability );
    const auto name                   = std::string_view("Test Thing");
    const std::int32_t hullhitpoints  = 2000;
    const std::int32_t mass           = 5000;
    const float build_complexity      = 2.5f;
    const auto categorisation         = OffensiveComponent;
    const auto build_cost             = Amount( Kamacite, 100 );
    const Component c(name,
                      hullhitpoints,
                      mass,
                      categorisation,
                      build_cost,
                      build_complexity);
    const Hardpoint hp( c, shooter );
    const ComponentOrientation dir = Rotated270;

    const Oriented_hardpoint ohp( dir, hp );

    nlohmann::json j = ohp;
    auto ohp2 = j.get<Oriented_hardpoint>();
    EXPECT_EQ( ohp, ohp2 );
}