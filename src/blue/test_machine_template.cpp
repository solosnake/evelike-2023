#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"
#include <fstream>

using namespace solosnake;
using namespace blue;

namespace
{
    std::unique_ptr<Blueprint> valid_bp()
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

        return std::make_unique<Blueprint>(chass, thrusters, softpoints, orientedhardpoints);
    }

    Instructions get_boot_code()
    {
        std::vector<Instruction> code;
        code.push_back( blue::compile( "goto 5             	// Jump past data blocks") );
        code.push_back( blue::compile( "0                	// Turn direction variable.") );
        code.push_back( blue::compile( "0                	// Turn count.") );
        code.push_back( blue::compile( "0                	// Movement count.") );
        code.push_back( blue::compile( "[2] = get_random( 0, 1 ) 	// Turn direction") );
        code.push_back( blue::compile( "[3] = get_random( 1, 5 ) 	// Turn count") );
        code.push_back( blue::compile( "[4] = get_random( 1, 4 ) 	// Movement") );
        code.push_back( blue::compile( "turn( [2], [3] )") );
        code.push_back( blue::compile( "advance( [4] )") );
        return Instructions(code);
    }
}

TEST( Machine_template, Machine_template )
{
    auto bp =  std::shared_ptr<Blueprint>(valid_bp());

    Machine_template mt;
    mt.machines_name = "ABC-1";
    mt.pulse_period = 3;
    mt.hue_shift = 5;
    mt.known_blueprints.push_back( bp );
    mt.known_blueprints.push_back( bp );
    mt.known_blueprints.push_back( bp );
    mt.machines_blueprint = bp;
    mt.boot_code = get_boot_code();

    nlohmann::json j = mt;
    auto mt2 = j.get<Machine_template>();
    EXPECT_EQ( mt, mt2 );
}

TEST( Machine_template, to_from_file )
{
    auto bp =  std::shared_ptr<Blueprint>(valid_bp());
    Machine_template mt;
    mt.machines_name = "ABC-1";
    mt.pulse_period = 3;
    mt.hue_shift = 5;
    mt.known_blueprints.push_back( bp );
    mt.known_blueprints.push_back( bp );
    mt.known_blueprints.push_back( bp );
    mt.machines_blueprint = bp;
    mt.boot_code = get_boot_code();

    nlohmann::json j = mt;

    std::ofstream out_file("machine_template_test.json");
    out_file << std::setw(4) << j << std::endl;
    out_file.close();

    std::ifstream in_file("machine_template_test.json");
    auto j2 = nlohmann::json::parse(in_file);
    auto mt2 = j2.get<Machine_template>();
    EXPECT_EQ( mt, mt2 );
}

