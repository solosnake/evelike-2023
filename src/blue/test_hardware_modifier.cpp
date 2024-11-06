#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardware_modifier.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Hardware_modifier, Hardware_modifier )
{
    Hardware_modifier h;
    h.delta_armour_hitpoints = 500;
    h.delta_cap_broadcast_cost_per_tile = 200;
    h.delta_cap_capacity = 300;
    h.delta_cap_firing_cost = 400;
    h.delta_cap_instruction_cost = 125;
    h.delta_cap_refill_per_tick = 5;
    h.delta_cap_refine_cost = 10;
    h.delta_cap_sensing_cost_per_tile = 15;
    h.delta_cap_solar_panels_count = 20;
    h.delta_cap_to_build_10_components = 25;
    h.delta_cap_transmit_cost_per_tile = 30;
    h.delta_cargohold_volume = 35;
    h.delta_em_resistance = 40;
    h.delta_hull_hitpoints = 45;
    h.delta_kinetic_resistance = 50;
    h.delta_max_broadcast_range = 55;
    h.delta_max_firing_range = 60;
    h.delta_max_sensing_radius = 65;
    h.delta_max_stored_messages = 70;
    h.delta_max_transmit_range = 75;
    h.delta_processor_speed_cycles_per_tick = 80;
    h.delta_radiation_resistance = 85;
    h.delta_refine_period = 90;
    h.delta_thermal_resistance = 95;
    h.delta_thrust_power = 100;
    h.delta_ticks_to_build_a_component = 105;
    h.delta_tiles_sensed_per_10_ticks = 110;
    h.delta_turn_power = 115;

    h.factor_processor_speed_cycles_per_tick = 0.5f;
    h.factor_max_sensing_radius = 0.51f;
    h.factor_tiles_sensed_per_10_ticks = 0.52f;
    h.factor_cap_solar_panels_count = 0.53f;
    h.factor_cap_capacity = 0.54f;
    h.factor_cap_instruction_cost = 0.55f;
    h.factor_cap_refill_per_tick = 0.56f;
    h.factor_cap_sensing_cost_per_tile = 0.57f;
    h.factor_cap_broadcast_cost_per_tile = 0.58f;
    h.factor_cap_transmit_cost_per_tile = 0.59f;
    h.factor_cap_firing_cost = 0.555f;
    h.factor_cap_refine_cost = 0.556f;
    h.factor_max_firing_range = 0.557f;
    h.factor_thrust_power = 0.511f;
    h.factor_turn_power = 0.522f;
    h.factor_hull_hitpoints = 0.533f;
    h.factor_armour_hitpoints = 0.544f;
    h.factor_max_stored_messages = 0.555f;
    h.factor_max_broadcast_range = 0.556f;
    h.factor_max_transmit_range = 0.566f;
    h.factor_cargohold_volume = 0.577f;
    h.factor_ticks_to_build_a_component = 0.588f;
    h.factor_cap_to_build_10_components = 0.599f;
    h.factor_kinetic_resistance = 0.6f;
    h.factor_thermal_resistance = 0.66f;
    h.factor_em_resistance = 0.77f;
    h.factor_radiation_resistance = 0.88f;
    h.factor_refine_period = 0.99f;

    EXPECT_TRUE( h == h );
    EXPECT_FALSE( h != h );

    Hardware_modifier h1;
    EXPECT_FALSE( h == h1 );
    EXPECT_TRUE( h != h1 );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = h;
    auto h2 = j.get<Hardware_modifier>();
    EXPECT_EQ( h, h2 );
}
