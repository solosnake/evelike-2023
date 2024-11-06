#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include "solosnake/nlohmann/json.hpp"
#include "solosnake/blue/blue_bgra.hpp"
#include "solosnake/blue/blue_attachpoint.hpp"
#include "solosnake/blue/blue_blueprint.hpp"
#include "solosnake/blue/blue_board_state.hpp"
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_combat_system.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_component.hpp"
#include "solosnake/blue/blue_componentcategory.hpp"
#include "solosnake/blue/blue_emissive_channels.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hardpoint_shooter.hpp"
#include "solosnake/blue/blue_hardware_modifier.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_instruction.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_oriented_hardpoint.hpp"
#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_thruster_attachpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    void to_json(nlohmann::json& j, const Bgra& c)
    {
        const std::array<std::uint8_t, 4> colours{ c.blue(), c.green(), c.red(), c.alpha() };
        j = nlohmann::json{
                {"BGRA", colours}
            };
    }

    void from_json(const nlohmann::json& j, Bgra& c)
    {
        std::array<std::uint8_t, 4> colours{};
        j.at("BGRA").get_to(colours);
        c = Bgra(colours[0], colours[1], colours[2], colours[3]);
    }

    /// Class friended to all types that can be serialized to/from JSON.
    class to_from_json
    {
    public:
        static void to_json(nlohmann::json&, const Hex_grid&);
        static void from_json(const nlohmann::json&, Hex_grid&);

        static void to_json(nlohmann::json&, const Game_hex_grid&);
        static void from_json(const nlohmann::json&, Game_hex_grid&);

        static void to_json(nlohmann::json&, const Machine_name&);
        static void from_json(const nlohmann::json&, Machine_name&);

        static void from_json(const nlohmann::json&, std::unique_ptr<Machine>&);

        static void to_json(nlohmann::json&, const Chassis&);
        static void from_json(const nlohmann::json&, Chassis&);

        static void to_json(nlohmann::json&, const Emissive_channels&);
        static void from_json(const nlohmann::json&, Emissive_channels&);

        static void to_json(nlohmann::json&, const AttachPoint&);
        static void from_json(const nlohmann::json&, AttachPoint&);

        static void to_json(nlohmann::json&, const Thruster_attachpoint&);
        static void from_json(const nlohmann::json&, Thruster_attachpoint&);

        static void to_json(nlohmann::json&, const ComponentCategory&);
        static void from_json(const nlohmann::json&, ComponentCategory&);

        static void to_json(nlohmann::json&, const ComponentOrientation&);
        static void from_json(const nlohmann::json&, ComponentOrientation&);

        static void to_json(nlohmann::json&, const Hardpoint_shot&);
        static void from_json(const nlohmann::json&, Hardpoint_shot&);

        static void to_json(nlohmann::json&, const Hardware_modifier&);
        static void from_json(const nlohmann::json&, Hardware_modifier&);

        static void to_json(nlohmann::json&, const Hardware_factor&);
        static void from_json(const nlohmann::json&, Hardware_factor&);

        static void to_json(nlohmann::json&, const Tradables&);
        static void from_json(const nlohmann::json&, Tradables&);

        static void to_json(nlohmann::json&, const Amount&);
        static void from_json(const nlohmann::json&, Amount&);

        static void to_json(nlohmann::json&, const Instructions&);
        static void from_json(const nlohmann::json&, Instructions&);

        static void to_json(nlohmann::json&, const Board_state&);
        static Board_state boardstate_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Hardpoint_shooter&);
        static Hardpoint_shooter hardpointshooter_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Blueprint&);
        static Blueprint blueprint_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Component&);
        static Component component_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Softpoint&);
        static Softpoint softpoint_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Hardpoint&);
        static Hardpoint hardpoint_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Oriented_hardpoint&);
        static Oriented_hardpoint orientedhardpoint_from_json(const nlohmann::json&);

        static void to_json(nlohmann::json&, const Thruster&);
        static Thruster thruster_from_json(const nlohmann::json&);
    };

    ////////////////////////////////////////////////////////////////////////////

    void to_from_json::to_json(nlohmann::json& j, const Hex_grid& h)
    {
        j = nlohmann::json{
                {"grid", h.grid_},
                {"grid-width", h.width_},
                {"grid-height", h.height_},
                {"invalid-tile-value", h.invalid_tile_value_}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Hex_grid& h)
    {
        std::vector<HexContentFlags> grid;
        std::int8_t width{0};
        std::int8_t height{0};
        HexContentFlags invalidTile{};
        j.at("grid").get_to(grid);
        j.at("grid-width").get_to(width);
        j.at("grid-height").get_to(height);
        j.at("invalid-tile-value").get_to(invalidTile);
        h = Hex_grid(std::move(grid), width, height, invalidTile);
    }

    void to_from_json::to_json(nlohmann::json& j, const Game_hex_grid& g)
    {
        j = nlohmann::json{
                {"grid", g.grid()}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Game_hex_grid& g)
    {
        Hex_grid hg;
        j.at("grid").get_to(hg);
        g = Game_hex_grid(std::move(hg));
    }

    void to_from_json::to_json(nlohmann::json& j, const Machine_name& n)
    {
        j = nlohmann::json{
                {"machine-name", std::string_view(n)}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Machine_name& n)
    {
        std::string name;
        j.at("machine-name").get_to(name);
        n = Machine_name(name);
    }

    void to_from_json::from_json(const nlohmann::json& j, std::unique_ptr<Machine>& m)
    {
        Machine_template mt;
        j.at("machine").at("machine-template").get_to(mt);
        auto mm = Machine::create_machine( std::move(mt) );
        m.swap( mm );
    }

    void to_from_json::to_json(nlohmann::json& j, const Chassis& c)
    {
        j = nlohmann::json{
                {"chassis-name", c.name()},
                {"chassis-channels", c.channels()}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Chassis& c)
    {
        std::string name;
        Emissive_channels channels;
        j.at("chassis-name").get_to(name);
        j.at("chassis-channels").get_to(channels);
        c = Chassis(name, channels);
    }

    void to_from_json::to_json(nlohmann::json& j, const Emissive_channels& e)
    {
        j = nlohmann::json{
                {"colour-0", e.emissive_colours[0]},
                {"colour-1", e.emissive_colours[1]},
                {"readout-0-0", e.emissive_readouts[0][0]},
                {"readout-0-1", e.emissive_readouts[0][1]},
                {"readout-1-0", e.emissive_readouts[1][0]},
                {"readout-1-1", e.emissive_readouts[1][1]},
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Emissive_channels& e)
    {
        j.at("colour-0").get_to(e.emissive_colours[0]);
        j.at("colour-1").get_to(e.emissive_colours[1]);
        j.at("readout-0-0").get_to(e.emissive_readouts[0][0]);
        j.at("readout-0-1").get_to(e.emissive_readouts[0][1]);
        j.at("readout-1-0").get_to(e.emissive_readouts[1][0]);
        j.at("readout-1-1").get_to(e.emissive_readouts[1][1]);
    }

    void to_from_json::to_json(nlohmann::json& j, const AttachPoint& ap)
    {
        j = nlohmann::json{
                {"attachpoint", ap.location_number()}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, AttachPoint& ap)
    {
        unsigned short n = 0U;
        j.at("attachpoint").get_to(n);
        ap = AttachPoint(n);
    }

    void to_from_json::to_json(nlohmann::json& j, const Thruster_attachpoint& tap)
    {
        j = nlohmann::json{
                {"thruster-attachpoint", tap.location_number()}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Thruster_attachpoint& tap)
    {
        unsigned short n = 0U;
        j.at("thruster-attachpoint").get_to(n);
        tap = Thruster_attachpoint(n);
    }

    void to_from_json::to_json(nlohmann::json& j, const ComponentCategory& c)
    {
        j = nlohmann::json{
                {"component-category", blue::to_string(c)}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, ComponentCategory& c)
    {
        std::string text;
        j.at("component-category").get_to(text);
        c = blue::component_category_from_string(text);
    }

    void to_from_json::to_json(nlohmann::json& j, const ComponentOrientation& c)
    {
        j = nlohmann::json{
                {"component-orientation", blue::to_string(c)}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, ComponentOrientation& c)
    {
        std::string text;
        j.at("component-orientation").get_to(text);
        c = blue::component_orientation_from_string(text);
    }

    void to_from_json::to_json(nlohmann::json& j, const Hardpoint_shot& h)
    {
        switch(h.hardpoint_type_)
        {
            case WeaponHardpoint:
                j = nlohmann::json{
                        { "hardpoint-type",    blue::to_string(WeaponHardpoint)},
                        { "hardpoint-index",   h.hardpoint_index_},
                        { "max-range",         h.max_range_},
                        { "kinetic-damage",    h.shot_effect_.weapon.damage.kinetic_dmg() },
                        { "thermal-damage",    h.shot_effect_.weapon.damage.thermal_dmg() },
                        { "em-damage",         h.shot_effect_.weapon.damage.em_dmg() },
                        { "radiation-damage",  h.shot_effect_.weapon.damage.radiation_dmg() }
                    };
                break;

            case CapDrainHardpoint:
                j = nlohmann::json{
                        { "hardpoint-type",  blue::to_string(CapDrainHardpoint)},
                        { "hardpoint-index", h.hardpoint_index_},
                        { "max-range",       h.max_range_},
                        { "cap-drained",     h.shot_effect_.cap_drain.cap_drained }
                    };
                break;

            case CapXFerHardpoint:
                j = nlohmann::json{
                        { "hardpoint-type",  blue::to_string(CapXFerHardpoint)},
                        { "hardpoint-index", h.hardpoint_index_},
                        { "max-range",       h.max_range_},
                        { "cap-transferred", h.shot_effect_.cap_xfer.cap_transferred}
                    };
                break;

            case ArmourRepairerHardpoint:
                j = nlohmann::json{
                        { "hardpoint-type",  blue::to_string(ArmourRepairerHardpoint)},
                        { "hardpoint-index", h.hardpoint_index_},
                        { "max-range",       h.max_range_},
                        { "armor-repped",    h.shot_effect_.repairs.armour_repaired}
                    };
                break;

            case MinerHardpoint:
                j = nlohmann::json{
                        { "hardpoint-type",  blue::to_string(ArmourRepairerHardpoint)},
                        { "hardpoint-index", h.hardpoint_index_},
                        { "max-range",       h.max_range_},
                        { "volume-mined",    h.shot_effect_.mining.volume_mined}
                    };
                break;

            default:
                ss_throw("Bad hardpointshot type.");
        }
    }

    void to_from_json::from_json(const nlohmann::json& j, Hardpoint_shot& h)
    {
        std::string   type;
        std::uint8_t  hardpoint_index{0u};
        std::uint16_t max_range{0u};
        Shot_effects   shot_effect;

        j.at( "hardpoint-type").get_to(type);
        j.at( "hardpoint-index").get_to(hardpoint_index);
        j.at( "max-range").get_to(max_range);

        auto hardpoint_type = blue::hardpoint_type_from_string(type);

        switch(hardpoint_type)
        {
            case WeaponHardpoint:
                j.at( "kinetic-damage").get_to(shot_effect.weapon.damage.dmg_type[Combat_system::Kinetic]);
                j.at( "thermal-damage").get_to(shot_effect.weapon.damage.dmg_type[Combat_system::Thermal]);
                j.at( "em-damage").get_to(shot_effect.weapon.damage.dmg_type[Combat_system::ElectroMagnetic]);
                j.at( "radiation-damage").get_to(shot_effect.weapon.damage.dmg_type[Combat_system::Radiation]);
                break;

            case CapDrainHardpoint:
                j.at( "cap-drained").get_to(shot_effect.cap_drain.cap_drained);
                break;

            case CapXFerHardpoint:
                j.at( "cap-transferred").get_to(shot_effect.cap_xfer.cap_transferred);
                break;

            case ArmourRepairerHardpoint:
                j.at( "armor-repped").get_to(shot_effect.repairs.armour_repaired);
                break;

            case MinerHardpoint:
                j.at( "volume-mined").get_to(shot_effect.mining.volume_mined);
                break;

            default:
                ss_unreachable;
                break;

        }

        h.hardpoint_index_ = hardpoint_index;
        h.hardpoint_type_  = hardpoint_type;
        h.max_range_       = max_range;
        h.shot_effect_     = shot_effect;
    }

    void to_from_json::to_json(nlohmann::json& j, const Hardware_modifier& h)
    {
        j = nlohmann::json{
                { "delta_processor_speed_cycles_per_tick",    h.delta_processor_speed_cycles_per_tick },
                { "delta_max_sensing_radius",                 h.delta_max_sensing_radius },
                { "delta_tiles_sensed_per_10_ticks",          h.delta_tiles_sensed_per_10_ticks },
                { "delta_cap_solar_panels_count",             h.delta_cap_solar_panels_count },
                { "delta_cap_capacity",                       h.delta_cap_capacity },
                { "delta_cap_instruction_cost",               h.delta_cap_instruction_cost },
                { "delta_cap_refill_per_tick",                h.delta_cap_refill_per_tick },
                { "delta_cap_sensing_cost_per_tile",          h.delta_cap_sensing_cost_per_tile },
                { "delta_cap_broadcast_cost_per_tile",        h.delta_cap_broadcast_cost_per_tile },
                { "delta_cap_transmit_cost_per_tile",         h.delta_cap_transmit_cost_per_tile },
                { "delta_cap_firing_cost",                    h.delta_cap_firing_cost },
                { "delta_cap_refine_cost",                    h.delta_cap_refine_cost },
                { "delta_max_firing_range",                   h.delta_max_firing_range },
                { "delta_thrust_power",                       h.delta_thrust_power },
                { "delta_turn_power",                         h.delta_turn_power },
                { "delta_hull_hitpoints",                     h.delta_hull_hitpoints },
                { "delta_armour_hitpoints",                   h.delta_armour_hitpoints },
                { "delta_max_stored_messages",                h.delta_max_stored_messages },
                { "delta_max_broadcast_range",                h.delta_max_broadcast_range },
                { "delta_max_transmit_range",                 h.delta_max_transmit_range },
                { "delta_cargohold_volume",                   h.delta_cargohold_volume },
                { "delta_ticks_to_build_a_component",         h.delta_ticks_to_build_a_component },
                { "delta_cap_to_build_10_components",         h.delta_cap_to_build_10_components },
                { "delta_kinetic_resistance",                 h.delta_kinetic_resistance },
                { "delta_thermal_resistance",                 h.delta_thermal_resistance },
                { "delta_em_resistance",                      h.delta_em_resistance },
                { "delta_radiation_resistance",               h.delta_radiation_resistance },
                { "delta_refine_period",                      h.delta_refine_period },
                { "factor_processor_speed_cycles_per_tick",   h.factor_processor_speed_cycles_per_tick },
                { "factor_max_sensing_radius",                h.factor_max_sensing_radius },
                { "factor_tiles_sensed_per_10_ticks",         h.factor_tiles_sensed_per_10_ticks },
                { "factor_cap_solar_panels_count",            h.factor_cap_solar_panels_count },
                { "factor_cap_capacity",                      h.factor_cap_capacity },
                { "factor_cap_instruction_cost",              h.factor_cap_instruction_cost },
                { "factor_cap_refill_per_tick",               h.factor_cap_refill_per_tick },
                { "factor_cap_sensing_cost_per_tile",         h.factor_cap_sensing_cost_per_tile },
                { "factor_cap_broadcast_cost_per_tile",       h.factor_cap_broadcast_cost_per_tile },
                { "factor_cap_transmit_cost_per_tile",        h.factor_cap_transmit_cost_per_tile },
                { "factor_cap_firing_cost",                   h.factor_cap_firing_cost },
                { "factor_cap_refine_cost",                   h.factor_cap_refine_cost },
                { "factor_max_firing_range",                  h.factor_max_firing_range },
                { "factor_thrust_power",                      h.factor_thrust_power },
                { "factor_turn_power",                        h.factor_turn_power },
                { "factor_hull_hitpoints",                    h.factor_hull_hitpoints },
                { "factor_armour_hitpoints",                  h.factor_armour_hitpoints },
                { "factor_max_stored_messages",               h.factor_max_stored_messages },
                { "factor_max_broadcast_range",               h.factor_max_broadcast_range },
                { "factor_max_transmit_range",                h.factor_max_transmit_range },
                { "factor_cargohold_volume",                  h.factor_cargohold_volume },
                { "factor_ticks_to_build_a_component",        h.factor_ticks_to_build_a_component },
                { "factor_cap_to_build_10_components",        h.factor_cap_to_build_10_components },
                { "factor_kinetic_resistance",                h.factor_kinetic_resistance },
                { "factor_thermal_resistance",                h.factor_thermal_resistance },
                { "factor_em_resistance",                     h.factor_em_resistance },
                { "factor_radiation_resistance",              h.factor_radiation_resistance },
                { "factor_refine_period",                     h.factor_refine_period }
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Hardware_modifier& h)
    {
        j.at( "delta_processor_speed_cycles_per_tick").get_to(   h.delta_processor_speed_cycles_per_tick );
        j.at( "delta_max_sensing_radius").get_to(                h.delta_max_sensing_radius );
        j.at( "delta_tiles_sensed_per_10_ticks").get_to(         h.delta_tiles_sensed_per_10_ticks );
        j.at( "delta_cap_solar_panels_count").get_to(            h.delta_cap_solar_panels_count );
        j.at( "delta_cap_capacity").get_to(                      h.delta_cap_capacity );
        j.at( "delta_cap_instruction_cost").get_to(              h.delta_cap_instruction_cost );
        j.at( "delta_cap_refill_per_tick").get_to(               h.delta_cap_refill_per_tick );
        j.at( "delta_cap_sensing_cost_per_tile").get_to(         h.delta_cap_sensing_cost_per_tile );
        j.at( "delta_cap_broadcast_cost_per_tile").get_to(       h.delta_cap_broadcast_cost_per_tile );
        j.at( "delta_cap_transmit_cost_per_tile").get_to(        h.delta_cap_transmit_cost_per_tile );
        j.at( "delta_cap_firing_cost").get_to(                   h.delta_cap_firing_cost );
        j.at( "delta_cap_refine_cost").get_to(                   h.delta_cap_refine_cost );
        j.at( "delta_max_firing_range").get_to(                  h.delta_max_firing_range );
        j.at( "delta_thrust_power").get_to(                      h.delta_thrust_power );
        j.at( "delta_turn_power").get_to(                        h.delta_turn_power );
        j.at( "delta_hull_hitpoints").get_to(                    h.delta_hull_hitpoints );
        j.at( "delta_armour_hitpoints").get_to(                  h.delta_armour_hitpoints );
        j.at( "delta_max_stored_messages").get_to(               h.delta_max_stored_messages );
        j.at( "delta_max_broadcast_range").get_to(               h.delta_max_broadcast_range );
        j.at( "delta_max_transmit_range").get_to(                h.delta_max_transmit_range );
        j.at( "delta_cargohold_volume").get_to(                  h.delta_cargohold_volume );
        j.at( "delta_ticks_to_build_a_component").get_to(        h.delta_ticks_to_build_a_component );
        j.at( "delta_cap_to_build_10_components").get_to(        h.delta_cap_to_build_10_components );
        j.at( "delta_kinetic_resistance").get_to(                h.delta_kinetic_resistance );
        j.at( "delta_thermal_resistance").get_to(                h.delta_thermal_resistance );
        j.at( "delta_em_resistance").get_to(                     h.delta_em_resistance );
        j.at( "delta_radiation_resistance").get_to(              h.delta_radiation_resistance );
        j.at( "delta_refine_period").get_to(                     h.delta_refine_period );
        j.at( "factor_processor_speed_cycles_per_tick").get_to(  h.factor_processor_speed_cycles_per_tick );
        j.at( "factor_max_sensing_radius").get_to(               h.factor_max_sensing_radius );
        j.at( "factor_tiles_sensed_per_10_ticks").get_to(        h.factor_tiles_sensed_per_10_ticks );
        j.at( "factor_cap_solar_panels_count").get_to(           h.factor_cap_solar_panels_count );
        j.at( "factor_cap_capacity").get_to(                     h.factor_cap_capacity );
        j.at( "factor_cap_instruction_cost").get_to(             h.factor_cap_instruction_cost );
        j.at( "factor_cap_refill_per_tick").get_to(              h.factor_cap_refill_per_tick );
        j.at( "factor_cap_sensing_cost_per_tile").get_to(        h.factor_cap_sensing_cost_per_tile );
        j.at( "factor_cap_broadcast_cost_per_tile").get_to(      h.factor_cap_broadcast_cost_per_tile );
        j.at( "factor_cap_transmit_cost_per_tile").get_to(       h.factor_cap_transmit_cost_per_tile );
        j.at( "factor_cap_firing_cost").get_to(                  h.factor_cap_firing_cost );
        j.at( "factor_cap_refine_cost").get_to(                  h.factor_cap_refine_cost );
        j.at( "factor_max_firing_range").get_to(                 h.factor_max_firing_range );
        j.at( "factor_thrust_power").get_to(                     h.factor_thrust_power );
        j.at( "factor_turn_power").get_to(                       h.factor_turn_power );
        j.at( "factor_hull_hitpoints").get_to(                   h.factor_hull_hitpoints );
        j.at( "factor_armour_hitpoints").get_to(                 h.factor_armour_hitpoints );
        j.at( "factor_max_stored_messages").get_to(              h.factor_max_stored_messages );
        j.at( "factor_max_broadcast_range").get_to(              h.factor_max_broadcast_range );
        j.at( "factor_max_transmit_range").get_to(               h.factor_max_transmit_range );
        j.at( "factor_cargohold_volume").get_to(                 h.factor_cargohold_volume );
        j.at( "factor_ticks_to_build_a_component").get_to(       h.factor_ticks_to_build_a_component );
        j.at( "factor_cap_to_build_10_components").get_to(       h.factor_cap_to_build_10_components );
        j.at( "factor_kinetic_resistance").get_to(               h.factor_kinetic_resistance );
        j.at( "factor_thermal_resistance").get_to(               h.factor_thermal_resistance );
        j.at( "factor_em_resistance").get_to(                    h.factor_em_resistance );
        j.at( "factor_radiation_resistance").get_to(             h.factor_radiation_resistance );
        j.at( "factor_refine_period").get_to(                    h.factor_refine_period );
    }

    void to_from_json::to_json(nlohmann::json& j, const Hardware_factor& h)
    {
        j = nlohmann::json{
                {"hardware-factor", static_cast<float>(h) }
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Hardware_factor& h)
    {
        float f{0.0f};
        j.at("hardware-factor").get_to(f);
        h = Hardware_factor(f);
    }

    void to_from_json::to_json(nlohmann::json& j, const Tradables& t)
    {
        j = nlohmann::json{
                {"amount",   t.as_amount()}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Tradables& t)
    {
        Amount a;
        j.at("amount").get_to(a);
        t = Tradables(a);
    }

    void to_from_json::to_json(nlohmann::json& j, const Amount& a)
    {
        j = nlohmann::json{
                {"Panguite",   a[Panguite]},
                {"Kamacite",   a[Kamacite]},
                {"Ataxite",    a[Ataxite]},
                {"Chondrite",  a[Chondrite]},
                {"Metals",     a[Metals]},
                {"NonMetals",  a[NonMetals]},
                {"SemiMetals", a[SemiMetals]},
                {"Alkalis",    a[Alkalis]}
            };
    }

    void to_from_json::from_json(const nlohmann::json& j, Amount& a)
    {
        std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT> contents{};
        j.at("Panguite").get_to(contents[Panguite]);
        j.at("Kamacite").get_to(contents[Kamacite]);
        j.at("Ataxite").get_to(contents[Ataxite]);
        j.at("Chondrite").get_to(contents[Chondrite]);
        j.at("Metals").get_to(contents[Metals]);
        j.at("NonMetals").get_to(contents[NonMetals]);
        j.at("SemiMetals").get_to(contents[SemiMetals]);
        j.at("Alkalis").get_to(contents[Alkalis]);
        a = Amount(contents);
    }

    void to_from_json::to_json(nlohmann::json& j, const Instructions& code)
    {
        auto& json_code = j["instructions"];

        json_code = nlohmann::json::array();

        // Note: 1 based indexing into `instructions`
        for( size_t i=1u; i <= code.size(); ++i )
        {
            ss_dbg( blue::decompile( code[i]) );
            json_code.push_back( blue::decompile( code[i]) );
        }
    }

    void to_from_json::from_json(const nlohmann::json& j, Instructions& code)
    {
        std::vector<Instruction> lines;
        const auto& json_code = j.at("instructions");
        lines.reserve( json_code.size() );

        for( size_t i=0u; i < json_code.size(); ++i )
        {
            ss_dbg( json_code.at(i).get<std::string>() );
            lines.push_back( blue::compile( json_code.at(i).get<std::string>() ) );
        }

        code = Instructions(std::move(lines));
    }


    ////////////////////////////////////////////////////////////////////////////

    void to_from_json::to_json(nlohmann::json& , const Board_state& )
    {
    }

    Board_state to_from_json::boardstate_from_json(const nlohmann::json& )
    {
        // TODO

        /*
        unsigned int                                frame_number_;
        boardobservers                              observers_;
        std::vector<Trade_request>                   trade_requests_;
        std::vector<Machine_event>                  machine_events_;
        std::vector<Secondary_event>                 secondary_events_;
        std::priority_queue<Delayed_machine_event>  delayedevents_;
        mutable std::vector<Hex_coord>               collisions_;
        std::vector<asteroid>                       asteroids_;
        std::vector<sun>                            suns_;
        std::vector<hexgrid::Hex_tile>              tile_buffer_; ///< Used as temp by some calls.
        Board_state_grid_view                          grid_;
        std::vector<team>                           teams_;
        solosnake::Good_rand                        randoms_;
        std::unique_ptr<Hex_pathfinder>              pathfinder_;
        */

       ss_throw("NOT IMPLEMENTED");
/*
        // Create default boardstate and then fill it correctly.
        std::vector<Asteroid> asteroids;
        std::vector<Sun> suns;
        Game_hex_grid hgrid;

        Board_state board(12, 34, std::move(hgrid), std::move(asteroids), std::move(suns) );

        return board;
*/
    }

    void to_from_json::to_json(nlohmann::json& j, const Hardpoint_shooter& s)
    {
        j = nlohmann::json{
                {"hardpoint-shot",           s.hardpoint_shot_ },
                {"rotation-speed",           s.rotation_speed_ },
                {"firing-cap-cost",          s.firing_cap_cost_ },
                {"firing-cooldown",          s.firing_cooldown_ },
                {"min-angle-degrees",        s.min_angle_degrees_ },
                {"max-angle-degrees",        s.max_angle_degrees_ },
                {"min-range",                s.min_range_ },
                {"max-range-percent-effect", s.max_range_percent_effect_ },
                {"reliability",              s.reliability_ }
            };
    }

    Hardpoint_shooter to_from_json::hardpointshooter_from_json(const nlohmann::json& j)
    {
        Hardpoint_shot   hardpointshot{};
        Fixed_angle      rotation_speed{};
        std::uint16_t    firing_cap_cost{};
        std::uint16_t    firing_cooldown{};
        std::int16_t     min_angle_degrees{};
        std::int16_t     max_angle_degrees{};
        std::uint16_t    min_range{};
        std::uint16_t    max_range_percent_effect{};
        std::uint8_t     reliability{};

        j.at("hardpoint-shot").get_to(hardpointshot);
        j.at("rotation-speed").get_to(rotation_speed);
        j.at("firing-cap-cost").get_to(firing_cap_cost);
        j.at("firing-cooldown").get_to(firing_cooldown);
        j.at("min-angle-degrees").get_to(min_angle_degrees);
        j.at("max-angle-degrees").get_to(max_angle_degrees);
        j.at("min-range").get_to(min_range);
        j.at("max-range-percent-effect").get_to(max_range_percent_effect);
        j.at("reliability").get_to(reliability);

        Hardpoint_shooter shooter( hardpointshot,
                                  rotation_speed,
                                  firing_cap_cost,
                                  firing_cooldown,
                                  min_angle_degrees,
                                  max_angle_degrees,
                                  min_range,
                                  max_range_percent_effect,
                                  reliability );

        return shooter;
    }

    void to_from_json::to_json(nlohmann::json& j, const Blueprint& b)
    {
        j = nlohmann::json{
                {"chassis",             b.chassis_ },
                {"thrusters",           b.make_thrustpoints_map() },
                {"softpoints",          b.make_softpoints_map() },
                {"orientedhardpoints",  b.make_oriented_hardpoints_map() }
            };
    }

    Blueprint to_from_json::blueprint_from_json(const nlohmann::json& j)
    {
        Chassis c;
        std::map<Thruster_attachpoint, Thruster> thrusters;
        std::map<AttachPoint, Softpoint> softpoints;
        std::map<AttachPoint, Oriented_hardpoint> orientedhardpoints;

        j.at( "chassis").get_to(c);
        j.at( "thrusters").get_to(thrusters);
        j.at( "softpoints").get_to(softpoints);
        j.at( "orientedhardpoints").get_to(orientedhardpoints);

        return Blueprint(c, thrusters, softpoints, orientedhardpoints);
    }

    void to_from_json::to_json(nlohmann::json& j, const Component& c)
    {
        j = nlohmann::json{
                {"component-name", c.component_name()},
                {"hull-hitpoints", c.hull_hitpoints()},
                {"mass", c.mass()},
                {"category", c.categorisation()},
                {"build-cost", c.build_cost()},
                {"build-complexity", c.build_complexity()}
            };
    }

    Component to_from_json::component_from_json(const nlohmann::json& j)
    {
        std::string_view name;
        std::int32_t hullhitpoints{0};
        std::int32_t mass{0};
        ComponentCategory category{};
        Amount build_cost;
        float build_complexity{1.0f};
        j.at( "component-name").get_to(name);
        j.at( "hull-hitpoints").get_to(hullhitpoints);
        j.at( "mass").get_to(mass);
        j.at( "category").get_to(category);
        j.at( "build-cost").get_to(build_cost);
        j.at( "build-complexity").get_to(build_complexity);
        return Component(name, hullhitpoints, mass, category, build_cost, build_complexity);
    }

    void to_from_json::to_json(nlohmann::json& j, const Softpoint& s)
    {
        j = nlohmann::json{
                { "hardware-modifier", s.modifier() },
                { "component",         s.details() }
        }   ;
    }

    Softpoint to_from_json::softpoint_from_json(const nlohmann::json& j)
    {
        Hardware_modifier m;
        j.at( "hardware-modifier").get_to(m);
        auto c = j.at( "component").get<Component>();
        return Softpoint(c, m);
    }

    void to_from_json::to_json(nlohmann::json& j, const Hardpoint& h)
    {
        j = nlohmann::json{
                { "hardpoint-shooter", h.shooter() },
                { "component",         h.details() }
        };
    }

    Hardpoint to_from_json::hardpoint_from_json(const nlohmann::json& j)
    {
        auto s = j.at( "hardpoint-shooter").get<Hardpoint_shooter>();
        auto c = j.at( "component").get<Component>();
        return Hardpoint(c, s);
    }

    void to_from_json::to_json(nlohmann::json& j, const Oriented_hardpoint& h)
    {
        j = nlohmann::json{
                { "hardpoint",   h.oriented_hardpoint() },
                { "orientation", h.hardpoint_orientation() }
        };
    }

    Oriented_hardpoint to_from_json::orientedhardpoint_from_json(const nlohmann::json& j)
    {
        auto h = j.at( "hardpoint").get<Hardpoint>();
        auto c = j.at( "orientation").get<ComponentOrientation>();
        return Oriented_hardpoint(c, h);
    }

    void to_from_json::to_json(nlohmann::json& j, const Thruster& h)
    {
        j = nlohmann::json{
                { "component", h.details() },
                { "power",     h.power() }
        };
    }

    Thruster to_from_json::thruster_from_json(const nlohmann::json& j)
    {
        auto c = j.at( "component").get<Component>();
        auto p = j.at( "power").get<std::int32_t>();
        return Thruster(c, p);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Classes with default ctors:

    void to_json(nlohmann::json& j, const Hex_grid& h)
    {
        to_from_json::to_json(j, h);
    }

    void from_json(const nlohmann::json& j, Hex_grid& h)
    {
        to_from_json::from_json(j, h);
    }

    void to_json(nlohmann::json& j, const Game_hex_grid& g)
    {
        to_from_json::to_json(j, g);
    }

    void from_json(const nlohmann::json& j, Game_hex_grid& g)
    {
        to_from_json::from_json(j, g);
    }

    void to_json(nlohmann::json& j, const Machine_name& n)
    {
        to_from_json::to_json(j, n);
    }

    void from_json(const nlohmann::json& j, Machine_name& n)
    {
        to_from_json::from_json(j, n);
    }

    void to_json(nlohmann::json& j, const Machine_template& mt)
    {
        if( nullptr == mt.machines_blueprint )
        {
            ss_throw("Machines blueprint is null.");
        }

        // Build array of known blueprints.
        auto known_bps = nlohmann::json::array();
        for( const auto& bp : mt.known_blueprints )
        {
            if( bp )
            {
                known_bps.push_back( *bp );
            }
            else
            {
                ss_throw("A known blueprint was null.");
            }
        }

        j = nlohmann::json{
                {"machines-name",      mt.machines_name },
                {"machines-blueprint", *mt.machines_blueprint },
                {"known-blueprints",   known_bps },
                {"boot-code",          mt.boot_code },
                {"pulse-period",       mt.pulse_period },
                {"hue-shift",          mt.hue_shift },
            };
    }

    void from_json(const nlohmann::json& j, Machine_template& mt)
    {
        std::vector<std::shared_ptr<Blueprint>> known_bps;
        known_bps.reserve( j.at("known-blueprints").size() );
        for(size_t i=0; i < j.at("known-blueprints").size(); ++i)
        {
            auto bp = std::make_shared<Blueprint>( j.at("known-blueprints").at(i).get<Blueprint>() ) ;
            known_bps.push_back( bp );
        }

        auto machine_bp = std::make_shared<Blueprint>( j.at("machines-blueprint").get<Blueprint>() ) ;

        j.at("machines-name").get_to(mt.machines_name);
        j.at("boot-code").get_to(mt.boot_code);
        j.at("pulse-period").get_to(mt.pulse_period);
        j.at("hue-shift").get_to(mt.hue_shift);
        mt.known_blueprints.swap( known_bps );
        mt.machines_blueprint.swap( machine_bp );

    }

    void from_json(const nlohmann::json& j, std::unique_ptr<Machine>& m)
    {
        to_from_json::from_json(j, m);
    }

    void to_json(nlohmann::json& j, const Chassis& c)
    {
        to_from_json::to_json(j, c);
    }

    void from_json(const nlohmann::json& j, Chassis& c)
    {
        to_from_json::from_json(j, c);
    }

    void to_json(nlohmann::json& j, const Emissive_channels& e)
    {
        to_from_json::to_json(j, e);
    }

    void from_json(const nlohmann::json& j, Emissive_channels& e)
    {
        to_from_json::from_json(j, e);
    }

    void to_json(nlohmann::json& j, const AttachPoint& ap)
    {
        to_from_json::to_json(j, ap);
    }

    void from_json(const nlohmann::json& j, AttachPoint& ap)
    {
        to_from_json::from_json(j, ap);
    }

    void to_json(nlohmann::json& j, const Thruster_attachpoint& tap)
    {
        to_from_json::to_json(j, tap);
    }

    void from_json(const nlohmann::json& j, Thruster_attachpoint& tap)
    {
        to_from_json::from_json(j, tap);
    }

    void to_json(nlohmann::json& j, const ComponentCategory& c)
    {
        to_from_json::to_json(j, c);
    }

    void from_json(const nlohmann::json& j, ComponentCategory& c)
    {
        to_from_json::from_json(j, c);
    }

    void to_json(nlohmann::json& j, const ComponentOrientation& c)
    {
        to_from_json::to_json(j, c);
    }

    void from_json(const nlohmann::json& j, ComponentOrientation& c)
    {
        to_from_json::from_json(j, c);
    }

    void to_json(nlohmann::json& j, const Hardpoint_shot& h)
    {
        to_from_json::to_json(j, h);
    }

    void from_json(const nlohmann::json& j, Hardpoint_shot& h)
    {
        to_from_json::from_json(j, h);
    }

    void to_json(nlohmann::json& j, const Hardware_modifier& m)
    {
        to_from_json::to_json(j, m);
    }

    void from_json(const nlohmann::json& j, Hardware_modifier& m)
    {
        to_from_json::from_json(j, m);
    }

    void to_json(nlohmann::json& j, const Hardware_factor& m)
    {
        to_from_json::to_json(j, m);
    }

    void from_json(const nlohmann::json& j, Hardware_factor& m)
    {
        to_from_json::from_json(j, m);
    }

    void to_json(nlohmann::json& j, const Tradables& t)
    {
        to_from_json::to_json(j, t);
    }

    void from_json(const nlohmann::json& j, Tradables& t)
    {
        to_from_json::from_json(j, t);
    }

    void to_json(nlohmann::json& j, const Amount& a)
    {
        to_from_json::to_json(j, a);
    }

    void from_json(const nlohmann::json& j, Amount& a)
    {
        to_from_json::from_json(j, a);
    }

    void to_json(nlohmann::json& j, const Instructions& a)
    {
        to_from_json::to_json(j, a);
    }

    void from_json(const nlohmann::json& j, Instructions& a)
    {
        to_from_json::from_json(j, a);
    }


    ////////////////////////////////////////////////////////////////////////////
    // Classes without default ctors:

    void to_json(nlohmann::json& j, const Board_state& b)
    {
        to_from_json::to_json(j, b);
    }

    Board_state boardstate_from_json(const nlohmann::json& j)
    {
        return to_from_json::boardstate_from_json(j);
    }

    void to_json(nlohmann::json& j, const Hardpoint_shooter& h)
    {
        to_from_json::to_json(j, h);
    }

    Hardpoint_shooter hardpointshooter_from_json(const nlohmann::json& j)
    {
        return to_from_json::hardpointshooter_from_json(j);
    }

    void to_json(nlohmann::json& j, const Blueprint& b)
    {
        to_from_json::to_json(j, b);
    }

    Blueprint blueprint_from_json(const nlohmann::json& j)
    {
        return to_from_json::blueprint_from_json(j);
    }

    void to_json(nlohmann::json& j, const Component& b)
    {
        to_from_json::to_json(j, b);
    }

    Component component_from_json(const nlohmann::json& j)
    {
        return to_from_json::component_from_json(j);
    }

    void to_json(nlohmann::json& j, const Softpoint& b)
    {
        to_from_json::to_json(j, b);
    }

    Softpoint softpoint_from_json(const nlohmann::json& j)
    {
        return to_from_json::softpoint_from_json(j);
    }

    void to_json(nlohmann::json& j, const Hardpoint& b)
    {
        to_from_json::to_json(j, b);
    }

    Hardpoint hardpoint_from_json(const nlohmann::json& j)
    {
        return to_from_json::hardpoint_from_json(j);
    }

    void to_json(nlohmann::json& j, const Oriented_hardpoint& b)
    {
        to_from_json::to_json(j, b);
    }

    Oriented_hardpoint orientedhardpoint_from_json(const nlohmann::json& j)
    {
        return to_from_json::orientedhardpoint_from_json(j);
    }

    void to_json(nlohmann::json& j, const Thruster& b)
    {
        to_from_json::to_json(j, b);
    }

    Thruster thruster_from_json(const nlohmann::json& j)
    {
        return to_from_json::thruster_from_json(j);
    }
}

namespace nlohmann
{
    ////////////////////////////////////////////////////////////////////////////
    // nlohmann::json serialisation for classes without default ctors.

    blue::Hardpoint_shooter adl_serializer<blue::Hardpoint_shooter>::from_json(const json& j)
    {
        return blue::hardpointshooter_from_json(j);
    }

    blue::Board_state adl_serializer<blue::Board_state>::from_json(const json& j)
    {
        return blue::boardstate_from_json(j);
    }

    blue::Blueprint adl_serializer<blue::Blueprint>::from_json(const json& j)
    {
        return blue::blueprint_from_json(j);
    }

    blue::Component adl_serializer<blue::Component>::from_json(const json& j)
    {
        return blue::component_from_json(j);
    }

    blue::Softpoint adl_serializer<blue::Softpoint>::from_json(const json& j)
    {
        return blue::softpoint_from_json(j);
    }

    blue::Hardpoint adl_serializer<blue::Hardpoint>::from_json(const json& j)
    {
        return blue::hardpoint_from_json(j);
    }

    blue::Oriented_hardpoint adl_serializer<blue::Oriented_hardpoint>::from_json(const json& j)
    {
        return blue::orientedhardpoint_from_json(j);
    }

    blue::Thruster adl_serializer<blue::Thruster>::from_json(const json& j)
    {
        return blue::thruster_from_json(j);
    }
}
