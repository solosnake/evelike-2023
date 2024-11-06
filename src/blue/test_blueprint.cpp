#include <map>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_attachpoint.hpp"
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_oriented_hardpoint.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_thruster_attachpoint.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace solosnake;
using namespace blue;


TEST( Blueprint, empty )
{
    const Chassis c;
    const std::map<Thruster_attachpoint, Thruster> thrusters;
    const std::map<AttachPoint, Softpoint> softpoints;
    const std::map<AttachPoint, Oriented_hardpoint> orientedhardpoints;

    EXPECT_THROW( Blueprint(c, thrusters, softpoints, orientedhardpoints) );
}

TEST( Blueprint, minimal )
{
    Chassis chass;
    std::map<Thruster_attachpoint, Thruster> thrusters;
    std::map<AttachPoint, Softpoint> softpoints;
    std::map<AttachPoint, Oriented_hardpoint> orientedhardpoints;

    Component c("FOO",
                1234,
                2000,
                IndustrialComponent,
                Amount( Chondrite, 123 ),
                2.3f);

    Hardware_modifier hm;

    softpoints.insert({AttachPoint(2u), Softpoint(c, hm)});

    Blueprint bp(chass, thrusters, softpoints, orientedhardpoints);
}


TEST( Blueprint, Blueprint )
{
    Chassis chass("ZZ1", Emissive_channels());

    std::map<Thruster_attachpoint, Thruster> thrusters;
    std::map<AttachPoint, Softpoint> softpoints;
    std::map<AttachPoint, Oriented_hardpoint> orientedhardpoints;

    Component c("FOO",
                1234,
                2000,
                IndustrialComponent,
                Amount( Chondrite, 123 ),
                2.3f);
    Hardware_modifier hm;

    softpoints.insert({AttachPoint(1u),  Softpoint(c, hm)});
    softpoints.insert({AttachPoint(20u), Softpoint(c, hm)});
    softpoints.insert({AttachPoint(40u), Softpoint(c, hm)});
    softpoints.insert({AttachPoint(60u), Softpoint(c, hm)});

    Component ct("Test",
                 12345,
                 234,
                 PropulsionComponent,
                 Amount( KamaciteOre, 55 ),
                 1.23f);
    std::int32_t power = 120;

    const Thruster t( ct, power );
    thrusters.insert({Thruster_attachpoint(81u), t});
    thrusters.insert({Thruster_attachpoint(83u), t});

    const Hardpoint_shot hardpointshot{ CapDrainHardpoint, 1u, 23u, {} };
    const Fixed_angle    rotation_speed{180};
    const std::uint16_t  firing_cap_cost{12u};
    const std::uint16_t  firing_cooldown{3u};
    const std::int16_t   min_angle_degrees{0};
    const std::int16_t   max_angle_degrees{180};
    const std::uint16_t  min_range{15u};
    const std::uint16_t  max_range_percent_effect{55u};
    const std::uint8_t   reliability{80u};
    const Hardpoint_shooter shooter( hardpointshot,
                                    rotation_speed,
                                    firing_cap_cost,
                                    firing_cooldown,
                                    min_angle_degrees,
                                    max_angle_degrees,
                                    min_range,
                                    max_range_percent_effect,
                                    reliability );

    const Component gun("BFG",
                        123,
                        45,
                        OffensiveComponent,
                        Amount( Kamacite, 100 ),
                        3.1f);

    const Hardpoint hp( gun, shooter );
    const ComponentOrientation dir = Rotated270;
    const Oriented_hardpoint ohp( dir, hp );

    orientedhardpoints.insert({AttachPoint(3), ohp});

    Blueprint bp(chass, thrusters, softpoints, orientedhardpoints);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = bp;
    //std::cout << j.dump(4) << std::endl;

    auto bp2 = j.get<Blueprint>();
    nlohmann::json j2 = bp2;
    //std::cout << j2.dump(4) << std::endl;

    EXPECT_EQ( bp.build_complexity(),               bp2.build_complexity() );
    EXPECT_EQ( bp.blueprint_chassis(),              bp2.blueprint_chassis() );
    EXPECT_EQ( bp.blueprint_chassis().name(),       bp2.blueprint_chassis().name() );
    EXPECT_EQ( bp.blueprint_chassis().channels(),   bp2.blueprint_chassis().channels() );
    EXPECT_EQ( bp.build_cost().volume(),            bp2.build_cost().volume() );
    EXPECT_EQ( bp.build_cost(),                     bp2.build_cost() );
    EXPECT_EQ( bp.component_count(),                bp2.component_count() );
    EXPECT_EQ( bp.hardpoint_count(),                bp2.hardpoint_count() );
    EXPECT_EQ( bp.softpoint_count(),                bp2.softpoint_count() );
    EXPECT_EQ( bp.thrusters_count(),                bp2.thrusters_count() );
    EXPECT_EQ( bp.hardware_of_blueprint(),          bp2.hardware_of_blueprint() );
    EXPECT_EQ( bp, bp2 );
}
