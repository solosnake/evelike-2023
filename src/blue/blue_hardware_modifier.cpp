#include "solosnake/blue/blue_hardware_modifier.hpp"

namespace blue
{
    Hardware_modifier& Hardware_modifier::operator+= (const Hardware_modifier& rhs)
    {
        delta_processor_speed_cycles_per_tick  += rhs.delta_processor_speed_cycles_per_tick ;
        delta_max_sensing_radius               += rhs.delta_max_sensing_radius              ;
        delta_tiles_sensed_per_10_ticks        += rhs.delta_tiles_sensed_per_10_ticks       ;
        delta_cap_solar_panels_count           += rhs.delta_cap_solar_panels_count          ;
        delta_cap_capacity                     += rhs.delta_cap_capacity                    ;
        delta_cap_instruction_cost             += rhs.delta_cap_instruction_cost            ;
        delta_cap_refill_per_tick              += rhs.delta_cap_refill_per_tick             ;
        delta_cap_sensing_cost_per_tile        += rhs.delta_cap_sensing_cost_per_tile       ;
        delta_cap_broadcast_cost_per_tile      += rhs.delta_cap_broadcast_cost_per_tile     ;
        delta_cap_transmit_cost_per_tile       += rhs.delta_cap_transmit_cost_per_tile      ;
        delta_cap_firing_cost                  += rhs.delta_cap_firing_cost                 ;
        delta_cap_refine_cost                  += rhs.delta_cap_refine_cost                 ;
        delta_max_firing_range                 += rhs.delta_max_firing_range                ;
        delta_thrust_power                     += rhs.delta_thrust_power                    ;
        delta_turn_power                       += rhs.delta_turn_power                      ;
        delta_hull_hitpoints                   += rhs.delta_hull_hitpoints                  ;
        delta_armour_hitpoints                 += rhs.delta_armour_hitpoints                ;
        delta_max_stored_messages              += rhs.delta_max_stored_messages             ;
        delta_max_broadcast_range              += rhs.delta_max_broadcast_range             ;
        delta_max_transmit_range               += rhs.delta_max_transmit_range              ;
        delta_cargohold_volume                 += rhs.delta_cargohold_volume                ;
        delta_ticks_to_build_a_component       += rhs.delta_ticks_to_build_a_component      ;
        delta_cap_to_build_10_components       += rhs.delta_cap_to_build_10_components      ;
        delta_kinetic_resistance               += rhs.delta_kinetic_resistance              ;
        delta_thermal_resistance               += rhs.delta_thermal_resistance              ;
        delta_em_resistance                    += rhs.delta_em_resistance                   ;
        delta_radiation_resistance             += rhs.delta_radiation_resistance            ;
        delta_refine_period                    += rhs.delta_refine_period                   ;

        factor_processor_speed_cycles_per_tick *= rhs.factor_processor_speed_cycles_per_tick;
        factor_max_sensing_radius              *= rhs.factor_max_sensing_radius             ;
        factor_tiles_sensed_per_10_ticks       *= rhs.factor_tiles_sensed_per_10_ticks      ;
        factor_cap_solar_panels_count          *= rhs.factor_cap_solar_panels_count         ;
        factor_cap_capacity                    *= rhs.factor_cap_capacity                   ;
        factor_cap_instruction_cost            *= rhs.factor_cap_instruction_cost           ;
        factor_cap_refill_per_tick             *= rhs.factor_cap_refill_per_tick            ;
        factor_cap_sensing_cost_per_tile       *= rhs.factor_cap_sensing_cost_per_tile      ;
        factor_cap_broadcast_cost_per_tile     *= rhs.factor_cap_broadcast_cost_per_tile    ;
        factor_cap_transmit_cost_per_tile      *= rhs.factor_cap_transmit_cost_per_tile     ;
        factor_cap_firing_cost                 *= rhs.factor_cap_firing_cost                ;
        factor_cap_refine_cost                 *= rhs.factor_cap_refine_cost                ;
        factor_max_firing_range                *= rhs.factor_max_firing_range               ;
        factor_thrust_power                    *= rhs.factor_thrust_power                   ;
        factor_turn_power                      *= rhs.factor_turn_power                     ;
        factor_hull_hitpoints                  *= rhs.factor_hull_hitpoints                 ;
        factor_armour_hitpoints                *= rhs.factor_armour_hitpoints               ;
        factor_max_stored_messages             *= rhs.factor_max_stored_messages            ;
        factor_max_broadcast_range             *= rhs.factor_max_broadcast_range            ;
        factor_max_transmit_range              *= rhs.factor_max_transmit_range             ;
        factor_cargohold_volume                *= rhs.factor_cargohold_volume               ;
        factor_ticks_to_build_a_component      *= rhs.factor_ticks_to_build_a_component     ;
        factor_cap_to_build_10_components      *= rhs.factor_cap_to_build_10_components     ;
        factor_kinetic_resistance              *= rhs.factor_kinetic_resistance             ;
        factor_thermal_resistance              *= rhs.factor_thermal_resistance             ;
        factor_em_resistance                   *= rhs.factor_em_resistance                  ;
        factor_radiation_resistance            *= rhs.factor_radiation_resistance           ;
        factor_refine_period                   *= rhs.factor_refine_period                  ;

        return *this;
    }

    bool Hardware_modifier::operator == ( const Hardware_modifier& rhs ) const noexcept
    {
        return (delta_processor_speed_cycles_per_tick  == rhs.delta_processor_speed_cycles_per_tick  )
            && (delta_max_sensing_radius               == rhs.delta_max_sensing_radius               )
            && (delta_tiles_sensed_per_10_ticks        == rhs.delta_tiles_sensed_per_10_ticks        )
            && (delta_cap_solar_panels_count           == rhs.delta_cap_solar_panels_count           )
            && (delta_cap_capacity                     == rhs.delta_cap_capacity                     )
            && (delta_cap_instruction_cost             == rhs.delta_cap_instruction_cost             )
            && (delta_cap_refill_per_tick              == rhs.delta_cap_refill_per_tick              )
            && (delta_cap_sensing_cost_per_tile        == rhs.delta_cap_sensing_cost_per_tile        )
            && (delta_cap_broadcast_cost_per_tile      == rhs.delta_cap_broadcast_cost_per_tile      )
            && (delta_cap_transmit_cost_per_tile       == rhs.delta_cap_transmit_cost_per_tile       )
            && (delta_cap_firing_cost                  == rhs.delta_cap_firing_cost                  )
            && (delta_cap_refine_cost                  == rhs.delta_cap_refine_cost                  )
            && (delta_max_firing_range                 == rhs.delta_max_firing_range                 )
            && (delta_thrust_power                     == rhs.delta_thrust_power                     )
            && (delta_turn_power                       == rhs.delta_turn_power                       )
            && (delta_hull_hitpoints                   == rhs.delta_hull_hitpoints                   )
            && (delta_armour_hitpoints                 == rhs.delta_armour_hitpoints                 )
            && (delta_max_stored_messages              == rhs.delta_max_stored_messages              )
            && (delta_max_broadcast_range              == rhs.delta_max_broadcast_range              )
            && (delta_max_transmit_range               == rhs.delta_max_transmit_range               )
            && (delta_cargohold_volume                 == rhs.delta_cargohold_volume                 )
            && (delta_ticks_to_build_a_component       == rhs.delta_ticks_to_build_a_component       )
            && (delta_cap_to_build_10_components       == rhs.delta_cap_to_build_10_components       )
            && (delta_kinetic_resistance               == rhs.delta_kinetic_resistance               )
            && (delta_thermal_resistance               == rhs.delta_thermal_resistance               )
            && (delta_em_resistance                    == rhs.delta_em_resistance                    )
            && (delta_radiation_resistance             == rhs.delta_radiation_resistance             )
            && (delta_refine_period                    == rhs.delta_refine_period                    )
            && (factor_processor_speed_cycles_per_tick == rhs.factor_processor_speed_cycles_per_tick )
            && (factor_max_sensing_radius              == rhs.factor_max_sensing_radius              )
            && (factor_tiles_sensed_per_10_ticks       == rhs.factor_tiles_sensed_per_10_ticks       )
            && (factor_cap_solar_panels_count          == rhs.factor_cap_solar_panels_count          )
            && (factor_cap_capacity                    == rhs.factor_cap_capacity                    )
            && (factor_cap_instruction_cost            == rhs.factor_cap_instruction_cost            )
            && (factor_cap_refill_per_tick             == rhs.factor_cap_refill_per_tick             )
            && (factor_cap_sensing_cost_per_tile       == rhs.factor_cap_sensing_cost_per_tile       )
            && (factor_cap_broadcast_cost_per_tile     == rhs.factor_cap_broadcast_cost_per_tile     )
            && (factor_cap_transmit_cost_per_tile      == rhs.factor_cap_transmit_cost_per_tile      )
            && (factor_cap_firing_cost                 == rhs.factor_cap_firing_cost                 )
            && (factor_cap_refine_cost                 == rhs.factor_cap_refine_cost                 )
            && (factor_max_firing_range                == rhs.factor_max_firing_range                )
            && (factor_thrust_power                    == rhs.factor_thrust_power                    )
            && (factor_turn_power                      == rhs.factor_turn_power                      )
            && (factor_hull_hitpoints                  == rhs.factor_hull_hitpoints                  )
            && (factor_armour_hitpoints                == rhs.factor_armour_hitpoints                )
            && (factor_max_stored_messages             == rhs.factor_max_stored_messages             )
            && (factor_max_broadcast_range             == rhs.factor_max_broadcast_range             )
            && (factor_max_transmit_range              == rhs.factor_max_transmit_range              )
            && (factor_cargohold_volume                == rhs.factor_cargohold_volume                )
            && (factor_ticks_to_build_a_component      == rhs.factor_ticks_to_build_a_component      )
            && (factor_cap_to_build_10_components      == rhs.factor_cap_to_build_10_components      )
            && (factor_kinetic_resistance              == rhs.factor_kinetic_resistance              )
            && (factor_thermal_resistance              == rhs.factor_thermal_resistance              )
            && (factor_em_resistance                   == rhs.factor_em_resistance                   )
            && (factor_radiation_resistance            == rhs.factor_radiation_resistance            )
            && (factor_refine_period                   == rhs.factor_refine_period                   )
            ;
    }

    bool Hardware_modifier::operator != ( const Hardware_modifier& rhs ) const noexcept
    {
        return !((*this) == rhs);
    }
}
