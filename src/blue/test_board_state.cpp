#include <iostream>
#include <memory>
#include <vector>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_good_rand.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

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
                    Amount(Chondrite, 123),
                    2.3f);

        Hardware_modifier hm;
        hm.delta_cap_capacity = 250;
        hm.delta_cap_refill_per_tick = 25;
        hm.delta_cap_instruction_cost = 1;
        hm.delta_max_firing_range = 2;
        hm.delta_processor_speed_cycles_per_tick = 16;

        softpoints.insert({AttachPoint(1u), Softpoint(c, hm)});
        softpoints.insert({AttachPoint(20u), Softpoint(c, hm)});
        softpoints.insert({AttachPoint(40u), Softpoint(c, hm)});
        softpoints.insert({AttachPoint(60u), Softpoint(c, hm)});

        Component ct("Test",
                     12345,
                     234,
                     PropulsionComponent,
                     Amount(KamaciteOre, 55),
                     1.23f);

        std::int32_t power = 120;
        const Thruster t(ct, power);
        thrusters.insert({Thruster_attachpoint(81u), t});
        thrusters.insert({Thruster_attachpoint(83u), t});

        const Hardpoint_shot hardpointshot{WeaponHardpoint, 1u, 23u, {}};
        const Fixed_angle rotation_speed{180};
        const std::uint16_t firing_cap_cost{12u};
        const std::uint16_t firing_cooldown{3u};
        const std::int16_t min_angle_degrees{0};
        const std::int16_t max_angle_degrees{180};
        const std::uint16_t min_range{15u};
        const std::uint16_t max_range_percent_effect{55u};
        const std::uint8_t reliability{80u};
        const Hardpoint_shooter shooter(hardpointshot,
                                        rotation_speed,
                                        firing_cap_cost,
                                        firing_cooldown,
                                        min_angle_degrees,
                                        max_angle_degrees,
                                        min_range,
                                        max_range_percent_effect,
                                        reliability);

        const Component gun("BFG",
                            123,
                            45,
                            OffensiveComponent,
                            Amount(Kamacite, 100),
                            3.1f);

        const Hardpoint hp(gun, shooter);
        const ComponentOrientation dir = Rotated270;
        const Oriented_hardpoint ohp(dir, hp);

        orientedhardpoints.insert({AttachPoint(3), ohp});

        auto bp = std::make_unique<Blueprint>(chass, thrusters, softpoints, orientedhardpoints);

        return bp;
    }

    Instructions get_boot_code()
    {
        constexpr std::string_view code =
            "0\n"
            "0\n"
            "0\n"
            "0\n"
            "0\n"
            "sense(2)\n"
            "[1] = get_sense_result_count()\n"
            "[2] = get_sense_result_location(1)\n"
            "[3] = has_line_of_fire_to([2])\n"
            "[5] = get_location()\n"
            "[4] = get_distance_between([2],[5])\n"
            "[3] = can_aim_at([2])\n"
            "aim_at([2])\n"
            "[3] = can_fire_at([2])\n"
            "fire_at([2])\n"
            "0							// Turn direction\n"
            "[16] = get_random( 0, 1 ) 	// Turn direction\n"
            "[16] = [16] + 3				// 3 or 4, 0 becomes 3, 3 % 2 = 1, 1 becomes 4, 4 % 2 = 0\n"
            "[16] = [16] % 2				// 0 or 1\n"
            "0                			// Turn count\n"
            "[20] = get_random( 1, 2 ) 	// Turn count\n"
            "turn( [16], [20] )\n"
            "[20] = get_random( 1, 5 ) 	// Advance count\n"
            "advance( [20] )\n"
            "goto 6";

        return Instructions(code);
    }

    std::unique_ptr<Machine> make_machine(const std::string &name)
    {
        Machine_template mt;
        mt.machines_name = Machine_name(name);
        mt.pulse_period = 3;
        mt.hue_shift = 5;
        mt.machines_blueprint = std::shared_ptr<Blueprint>(valid_bp());
        mt.boot_code = get_boot_code();

        return Machine::create_machine(std::move(mt));
    }
}

TEST(Board_state, Board_state)
{
    const unsigned int random_seed = 1234U;

    Hex_grid grid(10, 12, blue::empty_tile_value(), blue::off_board_tile_value());
    Game_hex_grid gamegrid(std::move(grid));

    Asteroid a22(OreTypes::AtaxiteOre, 20, Hex_coord::make_coord(3, 2));
    std::vector<Asteroid> asteroids;
    asteroids.push_back(a22);

    Sun s23(SunStrength::SunStrength3, SunType::SunType0, Hex_coord::make_coord(2, 3));
    std::vector<Sun> suns;
    suns.push_back(s23);

    Board_state board(random_seed,
                      std::move(gamegrid),
                      std::move(asteroids),
                      std::move(suns));

    board.add_machine_to_board(
        Hex_coord::make_coord(1, 1),
        HexFacingName::FacingTile0,
        make_machine("ABC-1"));

    board.add_machine_to_board(
        Hex_coord::make_coord(3, 3),
        HexFacingName::FacingTile1,
        make_machine("ABC-2"));

    Frame_events events;

    for (auto i = 0; i < 1000; ++i)
    {
        if( i == 200 )
        {
            // Add a machine and check later we see an event associated with
            // this machine.
            board.add_machine_to_board(
                Hex_coord::make_coord(6, 6),
                HexFacingName::FacingTile1,
                make_machine("XXX-2"));
        }

        board.advance_one_frame(events);

        if( i == 200 )
        {
            EXPECT_TRUE( events.machine_events.size() > 0 );

            bool machine_added_seen = false;

            for (auto e : events.machine_events)
            {
                switch (e.event_id())
                {
                    case EMachineAddedToBoard:
                        machine_added_seen = true;
                        EXPECT_TRUE(e.machine()->name() == std::string("XXX-2"));
                        break;
                    default:
                        break;
                }
            }

            EXPECT_TRUE( machine_added_seen );
        }

        if (events.machine_events.size() > 0)
        {
            for (auto e : events.machine_events)
            {
                switch (e.event_id())
                {
                case EMachineChangeTile:
                    std::cout << e.machine()->name() << " changed tile.\n";
                    break;

                case EMachineSensed:
                    std::cout << e.machine()->name() << " sensed.\n";
                    break;

                case EMachineFired:
                    std::cout << e.machine()->name() << " sensed.\n";
                    break;

                 default:
                    break;
                }
            }
        }

        std::cout.flush();
    }
}

TEST(Board_state, to_from_JSON)
{
    const unsigned int random_seed = 1234U;

    Hex_grid grid(5, 4, blue::empty_tile_value(), blue::off_board_tile_value());
    Game_hex_grid gamegrid(std::move(grid));

    Asteroid a22(OreTypes::AtaxiteOre, 20, Hex_coord::make_coord(2, 2));
    std::vector<Asteroid> asteroids;
    asteroids.push_back(a22);

    Sun s23(SunStrength::SunStrength3, SunType::SunType0, Hex_coord::make_coord(2, 3));
    std::vector<Sun> suns;
    suns.push_back(s23);

    Board_state board(random_seed,
                      std::move(gamegrid),
                      std::move(asteroids),
                      std::move(suns));

    // Serialise to/from JSON and check the objects are equal.
    // DISABLED nlohmann::json j = board;
    // DISABLED auto board2 = j.get<blue::Board_state>();
    // DISABLED EXPECT_EQ(board, board2);
}