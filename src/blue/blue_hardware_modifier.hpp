#ifndef blue_hardware_modifier_hpp
#define blue_hardware_modifier_hpp

#include "solosnake/blue/blue_hardware_factor.hpp"

namespace blue
{
    /// A modifier can do two things to each aspect of the Hardware: it can
    /// change its value, by adding to or subtracting from it, and it can
    /// apply a factor to it.
    /// For each element of the Hardware to which the modifiers apply, the
    /// formula is:
    ///     Hardware-Value = (HW.Base + (HWM.Change)) * (1 + HWM.Factor)
    ///
    /// A Softpoint is a Hardware_modifier.
    class Hardware_modifier
    {
    public:

        Hardware_modifier() = default;

        Hardware_modifier& operator+= ( const Hardware_modifier& );

        bool operator == ( const Hardware_modifier& ) const noexcept;
        bool operator != ( const Hardware_modifier& ) const noexcept;

        int                  delta_processor_speed_cycles_per_tick{0};
        int                  delta_max_sensing_radius{0};
        int                  delta_tiles_sensed_per_10_ticks{0};
        int                  delta_cap_solar_panels_count{0};
        int                  delta_cap_capacity{0};
        int                  delta_cap_instruction_cost{0};
        int                  delta_cap_refill_per_tick{0};
        int                  delta_cap_sensing_cost_per_tile{0};
        int                  delta_cap_broadcast_cost_per_tile{0};
        int                  delta_cap_transmit_cost_per_tile{0};
        int                  delta_cap_firing_cost{0};
        int                  delta_cap_refine_cost{0};
        int                  delta_max_firing_range{0};
        int                  delta_thrust_power{0};
        int                  delta_turn_power{0};
        int                  delta_hull_hitpoints{0};
        int                  delta_armour_hitpoints{0};
        int                  delta_max_stored_messages{0};
        int                  delta_max_broadcast_range{0};
        int                  delta_max_transmit_range{0};
        int                  delta_cargohold_volume{0};
        int                  delta_ticks_to_build_a_component{0};
        int                  delta_cap_to_build_10_components{0};
        int                  delta_kinetic_resistance{0};
        int                  delta_thermal_resistance{0};
        int                  delta_em_resistance{0};
        int                  delta_radiation_resistance{0};
        int                  delta_refine_period{0};

        Hardware_factor      factor_processor_speed_cycles_per_tick;
        Hardware_factor      factor_max_sensing_radius;
        Hardware_factor      factor_tiles_sensed_per_10_ticks;
        Hardware_factor      factor_cap_solar_panels_count;
        Hardware_factor      factor_cap_capacity;
        Hardware_factor      factor_cap_instruction_cost;
        Hardware_factor      factor_cap_refill_per_tick;
        Hardware_factor      factor_cap_sensing_cost_per_tile;
        Hardware_factor      factor_cap_broadcast_cost_per_tile;
        Hardware_factor      factor_cap_transmit_cost_per_tile;
        Hardware_factor      factor_cap_firing_cost;
        Hardware_factor      factor_cap_refine_cost;
        Hardware_factor      factor_max_firing_range;
        Hardware_factor      factor_thrust_power;
        Hardware_factor      factor_turn_power;
        Hardware_factor      factor_hull_hitpoints;
        Hardware_factor      factor_armour_hitpoints;
        Hardware_factor      factor_max_stored_messages;
        Hardware_factor      factor_max_broadcast_range;
        Hardware_factor      factor_max_transmit_range;
        Hardware_factor      factor_cargohold_volume;
        Hardware_factor      factor_ticks_to_build_a_component;
        Hardware_factor      factor_cap_to_build_10_components;
        Hardware_factor      factor_kinetic_resistance;
        Hardware_factor      factor_thermal_resistance;
        Hardware_factor      factor_em_resistance;
        Hardware_factor      factor_radiation_resistance;
        Hardware_factor      factor_refine_period;
    };
}

#endif
