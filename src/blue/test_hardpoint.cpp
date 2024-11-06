#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Hardpoint, Hardpoint )
{
    Hardpoint_shot hardpointshot{ CapDrainHardpoint, 1u, 23u, {} };

    Fixed_angle  rotation_speed{180};
    std::uint16_t firing_cap_cost{12u};
    std::uint16_t firing_cooldown{3u};
    std::int16_t  min_angle_degrees{0};
    std::int16_t  max_angle_degrees{180};
    std::uint16_t min_range{15u};
    std::uint16_t max_range_percent_effect{55u};
    std::uint8_t  reliability{80u};

    Hardpoint_shooter h( hardpointshot,
                        rotation_speed,
                        firing_cap_cost,
                        firing_cooldown,
                        min_angle_degrees,
                        max_angle_degrees,
                        min_range,
                        max_range_percent_effect,
                        reliability );

    auto name                   = std::string_view("Test Thing");
    std::int32_t hullhitpoints  = 2000;
    std::int32_t mass           = 5000;
    float build_complexity      = 2.5f;
    auto categorisation         = OffensiveComponent;
    auto build_cost             = Amount( Kamacite, 100 );

    Component c(name,
                hullhitpoints,
                mass,
                categorisation,
                build_cost,
                build_complexity);

    Hardpoint hp( c, h );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = hp;
    auto hp2 = j.get<Hardpoint>();
    EXPECT_EQ( hp, hp2 );
}
