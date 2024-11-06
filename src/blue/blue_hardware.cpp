#include <limits>
#include <climits>
#include <cassert>
#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_hardware_modifier.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    namespace
    {
        std::int32_t thrust_from_engine( const std::int32_t enginePower, const std::int32_t weight )
        {
            if( weight <= 0 )
            {
                ss_throw( "Hardware weight out of range." );
            }

            // We expect a figure around ~ 1000
            return static_cast<std::int32_t>( ( 1000.0f * enginePower ) / weight );
        }

        std::int32_t turn_from_engine( const std::int32_t enginePower,
                                       const std::int32_t weight,
                                       const float mobility )
        {
            if( weight <= 0 )
            {
                ss_throw( "Hardware weight out of range." );
            }

            if( mobility < 0.0f || mobility > 1.0f )
            {
                ss_throw( "Hardware mobility out of range." );
            }

            // We expect a figure around ~ 300
            return static_cast<std::int32_t>( ( 1000.0f * mobility * enginePower ) / weight );
        }

        template<typename T1, typename T2>
        void apply_delta( T1& lhs, T2 rhs )
        {
            if( rhs < 0 )
            {
                if( -rhs > static_cast<T2>( lhs ) )
                {
                    // Applying this would make lhs negative. Clamp to zero.
                    lhs = 0;
                }
                else
                {
                    lhs += rhs;
                }
            }
            else
            {
                T1 prev = lhs;
                lhs += rhs;
                if( prev > lhs )
                {
                    // We wrapped around. Clamp to  max.
                    lhs = std::numeric_limits<T1>::max();
                }
            }
        }

        template<typename T>
        void apply_factor( T& t, float f )
        {
            const float result = static_cast<float>( t ) * f;
            if( result > static_cast<float>( std::numeric_limits<T>::max() ) )
            {
                t = std::numeric_limits<T>::max();
            }
            else
            {

                t = static_cast<T>( result );
            }
        }
    }

    Hardware::Hardware()
        : cycles_per_instruction_( BLUE_DEFAULT_CYCLES_PER_INSTRUCTION )
        , processor_speed_cycles_per_tick_( 0u )
        , max_sensing_radius_( 0u )
        , tiles_sensed_per_10_ticks_( 0u )
        , cap_solar_panels_count_( 0u )
        , cap_capacity_( 0u )
        , cap_instruction_cost_( 0u )
        , cap_refill_per_tick_( 0u )
        , cap_sensing_cost_per_tile_( 0u )
        , cap_broadcast_cost_per_tile_( 0u )
        , cap_transmit_cost_per_tile_( 0u )
        , cap_firing_cost_( 0u )
        , cap_refine_cost_( 0u )
        , max_firing_range_( 0u )
        , thrust_power_( 0 )
        , turn_power_( 0 )
        , max_possible_speed_( 0 )
        , max_possible_turn_speed_( 0 )
        , hull_hit_points_( 0 )
        , armour_hitpoints_( 0 )
        , max_stored_messages_( 0u )
        , max_broadcast_range_( 0u )
        , max_transmit_range_( 0u )
        , cargohold_volume_( 0u )
        , ticks_to_build_a_component_( BLUE_DEFAULT_TICKS_TO_BUILD_1_COMPONENT )
        , cap_to_build_10_components_( BLUE_DEFAULT_CAP_TO_BUILD_10_COMPONENTS )
        , kinetic_resistance_( 0u )
        , thermal_resistance_( 0u )
        , em_resistance_( 0u )
        , radiation_resistance_( 0u )
        , refine_period_( 1u )
    {
    }

    Hardware::Hardware(
        size_t        default_cycles_per_instruction,
        float         default_mobility,
        std::int32_t  maxPossibleSpeed,
        std::int32_t  maxPossibleTurnSpeed,
        std::uint16_t cap_capacity,
        std::int32_t  enginePower,
        std::int32_t  weight,
        std::int32_t  hull_hitpoints )
        : cycles_per_instruction_( default_cycles_per_instruction )
        , processor_speed_cycles_per_tick_( 0u )
        , max_sensing_radius_( 0u )
        , tiles_sensed_per_10_ticks_( 0u )
        , cap_solar_panels_count_( 0u )
        , cap_capacity_( cap_capacity )
        , cap_instruction_cost_( 0u )
        , cap_refill_per_tick_( 0u )
        , cap_sensing_cost_per_tile_( 0u )
        , cap_broadcast_cost_per_tile_( 0u )
        , cap_transmit_cost_per_tile_( 0u )
        , cap_firing_cost_( 0u )
        , cap_refine_cost_( 0u )
        , max_firing_range_( 0u )
        , thrust_power_( thrust_from_engine( enginePower, weight ) )
        , turn_power_( turn_from_engine( enginePower, weight, default_mobility ) )
        , max_possible_speed_( maxPossibleSpeed )
        , max_possible_turn_speed_( maxPossibleTurnSpeed )
        , hull_hit_points_( hull_hitpoints )
        , armour_hitpoints_( 0 )
        , max_stored_messages_( 0u )
        , max_broadcast_range_( 0u )
        , max_transmit_range_( 0u )
        , cargohold_volume_( 0u )
        , ticks_to_build_a_component_( BLUE_DEFAULT_TICKS_TO_BUILD_1_COMPONENT )
        , cap_to_build_10_components_( BLUE_DEFAULT_CAP_TO_BUILD_10_COMPONENTS )
        , kinetic_resistance_( 0u )
        , thermal_resistance_( 0u )
        , em_resistance_( 0u )
        , radiation_resistance_( 0u )
        , refine_period_( 1u )
    {
    }

    void Hardware::validate() const
    {
        if( cycles_per_instruction_ == 0 )
        {
            ss_throw( "Cycles-per-Instruction is zero (impossible)." );
        }

        if( hull_hit_points_ == 0 )
        {
            ss_throw( "hull hit points is zero (impossible)." );
        }

        if( kinetic_resistance_ > 100 || thermal_resistance_ > 100 ||
                em_resistance_ > 100 || radiation_resistance_ > 100 )
        {
            ss_throw( "A hardware resistance was greater than 100%." );
        }

        if( refine_period_ == 0u )
        {
            ss_throw( "refine period is zero (impossible)." );
        }
    }

    void Hardware::remove_thruster_traits()
    {
        thrust_power_            = 0;
        turn_power_              = 0;
        max_possible_speed_      = 0;
        max_possible_turn_speed_ = 0;
    }

    /// Applies the modifier to the hardware and returns the result.
    Hardware Hardware::operator * ( const Hardware_modifier& m ) const
    {
        Hardware hw = *this;

        apply_delta( hw.processor_speed_cycles_per_tick_  , m.delta_processor_speed_cycles_per_tick );
        apply_delta( hw.max_sensing_radius_               , m.delta_max_sensing_radius );
        apply_delta( hw.tiles_sensed_per_10_ticks_        , m.delta_tiles_sensed_per_10_ticks );
        apply_delta( hw.cap_solar_panels_count_           , m.delta_cap_solar_panels_count );
        apply_delta( hw.cap_capacity_                     , m.delta_cap_capacity );
        apply_delta( hw.cap_instruction_cost_             , m.delta_cap_instruction_cost );
        apply_delta( hw.cap_refill_per_tick_              , m.delta_cap_refill_per_tick );
        apply_delta( hw.cap_sensing_cost_per_tile_        , m.delta_cap_sensing_cost_per_tile );
        apply_delta( hw.cap_broadcast_cost_per_tile_      , m.delta_cap_broadcast_cost_per_tile );
        apply_delta( hw.cap_transmit_cost_per_tile_       , m.delta_cap_transmit_cost_per_tile );
        apply_delta( hw.cap_firing_cost_                  , m.delta_cap_firing_cost );
        apply_delta( hw.cap_refine_cost_                  , m.delta_cap_refine_cost );
        apply_delta( hw.max_firing_range_                 , m.delta_max_firing_range );
        apply_delta( hw.thrust_power_                     , m.delta_thrust_power );
        apply_delta( hw.turn_power_                       , m.delta_turn_power );
        apply_delta( hw.hull_hit_points_                  , m.delta_hull_hitpoints );
        apply_delta( hw.armour_hitpoints_                 , m.delta_armour_hitpoints );
        apply_delta( hw.max_stored_messages_              , m.delta_max_stored_messages );
        apply_delta( hw.max_broadcast_range_              , m.delta_max_broadcast_range );
        apply_delta( hw.max_transmit_range_               , m.delta_max_transmit_range );
        apply_delta( hw.cargohold_volume_                 , m.delta_cargohold_volume );
        apply_delta( hw.ticks_to_build_a_component_       , m.delta_ticks_to_build_a_component );
        apply_delta( hw.cap_to_build_10_components_       , m.delta_cap_to_build_10_components );
        apply_delta( hw.kinetic_resistance_               , m.delta_kinetic_resistance );
        apply_delta( hw.thermal_resistance_               , m.delta_thermal_resistance );
        apply_delta( hw.em_resistance_                    , m.delta_em_resistance );
        apply_delta( hw.radiation_resistance_             , m.delta_radiation_resistance );
        apply_delta( hw.refine_period_                    , m.delta_refine_period );

        apply_factor( hw.processor_speed_cycles_per_tick_ , m.factor_processor_speed_cycles_per_tick );
        apply_factor( hw.max_sensing_radius_              , m.factor_max_sensing_radius );
        apply_factor( hw.tiles_sensed_per_10_ticks_       , m.factor_tiles_sensed_per_10_ticks );
        apply_factor( hw.cap_solar_panels_count_          , m.factor_cap_solar_panels_count );
        apply_factor( hw.cap_capacity_                    , m.factor_cap_capacity );
        apply_factor( hw.cap_instruction_cost_            , m.factor_cap_instruction_cost );
        apply_factor( hw.cap_refill_per_tick_             , m.factor_cap_refill_per_tick );
        apply_factor( hw.cap_sensing_cost_per_tile_       , m.factor_cap_sensing_cost_per_tile );
        apply_factor( hw.cap_broadcast_cost_per_tile_     , m.factor_cap_broadcast_cost_per_tile );
        apply_factor( hw.cap_transmit_cost_per_tile_      , m.factor_cap_transmit_cost_per_tile );
        apply_factor( hw.cap_firing_cost_                 , m.factor_cap_firing_cost );
        apply_factor( hw.cap_refine_cost_                 , m.factor_cap_refine_cost );
        apply_factor( hw.max_firing_range_                , m.factor_max_firing_range );
        apply_factor( hw.thrust_power_                    , m.factor_thrust_power );
        apply_factor( hw.turn_power_                      , m.factor_turn_power );
        apply_factor( hw.hull_hit_points_                 , m.factor_hull_hitpoints );
        apply_factor( hw.armour_hitpoints_                , m.factor_armour_hitpoints );
        apply_factor( hw.max_stored_messages_             , m.factor_max_stored_messages );
        apply_factor( hw.max_broadcast_range_             , m.factor_max_broadcast_range );
        apply_factor( hw.max_transmit_range_              , m.factor_max_transmit_range );
        apply_factor( hw.cargohold_volume_                , m.factor_cargohold_volume );
        apply_factor( hw.ticks_to_build_a_component_      , m.factor_ticks_to_build_a_component );
        apply_factor( hw.cap_to_build_10_components_      , m.factor_cap_to_build_10_components );
        apply_factor( hw.kinetic_resistance_              , m.factor_kinetic_resistance );
        apply_factor( hw.thermal_resistance_              , m.factor_thermal_resistance );
        apply_factor( hw.em_resistance_                   , m.factor_em_resistance );
        apply_factor( hw.radiation_resistance_            , m.factor_radiation_resistance );
        apply_factor( hw.refine_period_                   , m.factor_refine_period );

        hw.validate();

        return hw;
    }

    bool Hardware::operator == ( const Hardware& rhs ) const noexcept
    {
        return (cycles_per_instruction_ == rhs.cycles_per_instruction_)
            && (processor_speed_cycles_per_tick_ == rhs.processor_speed_cycles_per_tick_)
            && (max_sensing_radius_ == rhs.max_sensing_radius_)
            && (tiles_sensed_per_10_ticks_ == rhs.tiles_sensed_per_10_ticks_)
            && (cap_solar_panels_count_ == rhs.cap_solar_panels_count_)
            && (cap_capacity_ == rhs.cap_capacity_)
            && (cap_instruction_cost_ == rhs.cap_instruction_cost_)
            && (cap_refill_per_tick_ == rhs.cap_refill_per_tick_)
            && (cap_sensing_cost_per_tile_ == rhs.cap_sensing_cost_per_tile_)
            && (cap_broadcast_cost_per_tile_ == rhs.cap_broadcast_cost_per_tile_)
            && (cap_transmit_cost_per_tile_ == rhs.cap_transmit_cost_per_tile_)
            && (cap_firing_cost_ == rhs.cap_firing_cost_)
            && (cap_refine_cost_ == rhs.cap_refine_cost_)
            && (max_firing_range_ == rhs.max_firing_range_)
            && (thrust_power_ == rhs.thrust_power_)
            && (turn_power_ == rhs.turn_power_)
            && (max_possible_speed_ == rhs.max_possible_speed_)
            && (max_possible_turn_speed_ == rhs.max_possible_turn_speed_)
            && (hull_hit_points_ == rhs.hull_hit_points_)
            && (armour_hitpoints_ == rhs.armour_hitpoints_)
            && (max_stored_messages_ == rhs.max_stored_messages_)
            && (max_broadcast_range_ == rhs.max_broadcast_range_)
            && (max_transmit_range_ == rhs.max_transmit_range_)
            && (cargohold_volume_ == rhs.cargohold_volume_)
            && (ticks_to_build_a_component_ == rhs.ticks_to_build_a_component_)
            && (cap_to_build_10_components_ == rhs.cap_to_build_10_components_)
            && (kinetic_resistance_ == rhs.kinetic_resistance_)
            && (thermal_resistance_ == rhs.thermal_resistance_)
            && (em_resistance_ == rhs.em_resistance_)
            && (radiation_resistance_ == rhs.radiation_resistance_)
            && (refine_period_ == rhs.refine_period_)
            ;
    }

    bool Hardware::operator != ( const Hardware& rhs ) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
