#ifndef blue_hardware_hpp
#define blue_hardware_hpp

#include <cstdint>
#include <cstddef>

namespace blue
{
    class Hardware_modifier;

    /// Specifics of a Machine's internal capabilities. The Hardware values are
    /// determined by the Blueprint, based on the composition of the modules and
    /// Chassis of a Machine.
    class Hardware
    {
    public:

        Hardware();

        Hardware(
            size_t        default_cycles_per_instruction,
            float         default_mobility,
            std::int32_t  maxSpeed,
            std::int32_t  maxTurnSpeed,
            std::uint16_t maxcap,
            std::int32_t  enginePower,
            std::int32_t  weight,
            std::int32_t  hull_hitpoints );

        void validate() const;

        void remove_thruster_traits();

        bool is_mobile() const noexcept;

        Hardware operator * ( const Hardware_modifier& ) const;

        bool operator == ( const Hardware& rhs ) const noexcept;
        bool operator != ( const Hardware& rhs ) const noexcept;

        size_t            cycles_per_instruction() const noexcept          { return cycles_per_instruction_; }
        size_t            processor_speed_cycles_per_tick() const noexcept { return processor_speed_cycles_per_tick_; }
        std::uint16_t     tiles_sensed_per_10_ticks() const noexcept       { return tiles_sensed_per_10_ticks_; }
        std::uint16_t     cap_solar_panels_count() const noexcept          { return cap_solar_panels_count_; }
        std::uint16_t     cap_instruction_cost() const noexcept            { return cap_instruction_cost_; }
        std::uint16_t     cap_refill_per_tick() const noexcept             { return cap_refill_per_tick_; }
        std::uint16_t     cap_sensing_cost_per_tile() const noexcept       { return cap_sensing_cost_per_tile_; }
        std::uint16_t     cap_broadcast_cost_per_tile() const noexcept     { return cap_broadcast_cost_per_tile_; }
        std::uint16_t     cap_transmit_cost_per_tile() const noexcept      { return cap_transmit_cost_per_tile_; }
        std::uint16_t     cap_firing_cost() const noexcept                 { return cap_firing_cost_; }
        std::uint16_t     cap_refine_cost() const noexcept                 { return cap_refine_cost_; }
        std::int32_t      thrust_power() const noexcept                    { return thrust_power_; }
        std::int32_t      turn_power() const noexcept                      { return turn_power_; }
        std::int32_t      hull_hitpoints() const noexcept                  { return hull_hit_points_; }
        std::int32_t      armour_hitpoints() const noexcept                { return armour_hitpoints_; }
        std::uint16_t     cap_capacity() const noexcept                    { return cap_capacity_; }
        std::uint16_t     max_sensing_radius() const noexcept              { return max_sensing_radius_; }
        std::uint16_t     max_firing_range() const noexcept                { return max_firing_range_; }
        std::int32_t      max_possible_speed() const noexcept              { return max_possible_speed_; }
        std::int32_t      max_possible_turn_speed() const noexcept         { return max_possible_turn_speed_; }
        std::uint16_t     max_stored_messages() const noexcept             { return max_stored_messages_; }
        std::uint16_t     max_broadcast_range() const noexcept             { return max_broadcast_range_; }
        std::uint16_t     max_transmit_range() const noexcept              { return max_transmit_range_; }
        std::uint16_t     cargohold_volume() const noexcept                { return cargohold_volume_; }
        std::uint16_t     ticks_to_build_a_component() const noexcept      { return ticks_to_build_a_component_; }
        std::uint16_t     capcost_to_build_10_components() const noexcept  { return cap_to_build_10_components_; }
        std::uint16_t     kinetic_resistance() const noexcept              { return kinetic_resistance_; }
        std::uint16_t     thermal_resistance() const noexcept              { return thermal_resistance_; }
        std::uint16_t     em_resistance() const noexcept                   { return em_resistance_; }
        std::uint16_t     radiation_resistance() const noexcept            { return radiation_resistance_; }
        std::uint16_t     refine_period() const noexcept                   { return refine_period_; }

    private:

        size_t              cycles_per_instruction_;          ///< How many cycles an individual Instruction consumes. Low is good.
        size_t              processor_speed_cycles_per_tick_; ///< A measure of how fast the CPU is, this is how many cycles it does per tick. High is good.
        std::uint16_t       max_sensing_radius_;
        std::uint16_t       tiles_sensed_per_10_ticks_;
        std::uint16_t       cap_solar_panels_count_;
        std::uint16_t       cap_capacity_;
        std::uint16_t       cap_instruction_cost_;
        std::uint16_t       cap_refill_per_tick_;
        std::uint16_t       cap_sensing_cost_per_tile_;
        std::uint16_t       cap_broadcast_cost_per_tile_;
        std::uint16_t       cap_transmit_cost_per_tile_;
        std::uint16_t       cap_firing_cost_;
        std::uint16_t       cap_refine_cost_;               ///< Cost to refine a single unit of any ore.
        std::uint16_t       max_firing_range_;
        std::int32_t        thrust_power_;
        std::int32_t        turn_power_;
        std::int32_t        max_possible_speed_;            ///< Cannot be affected by h.w. modifiers.
        std::int32_t        max_possible_turn_speed_;       ///< Cannot be affected by h.w. modifiers.
        std::int32_t        hull_hit_points_;
        std::int32_t        armour_hitpoints_;
        std::uint16_t       max_stored_messages_;           ///< How many external broadcasts the Machine can store.
        std::uint16_t       max_broadcast_range_;           ///< How many tile rings it can broadcast over.
        std::uint16_t       max_transmit_range_;            ///< Max range to tile to transmit to.
        std::uint16_t       cargohold_volume_;              ///< How many volume units the Cargohold can take.
        std::uint16_t       ticks_to_build_a_component_;
        std::uint16_t       cap_to_build_10_components_;
        std::uint16_t       kinetic_resistance_;            ///< % Kinetic resistance.
        std::uint16_t       thermal_resistance_;            ///< % Thermal resistance.
        std::uint16_t       em_resistance_;                 ///< % Electronmagnetic resistance.
        std::uint16_t       radiation_resistance_;          ///< % Radiation resistance.
        std::uint16_t       refine_period_;                 ///< How many ticks to refine one unit of any ore.
    };

    //-------------------------------------------------------------------------

    inline bool Hardware::is_mobile() const noexcept
    {
        return max_possible_speed_ > 0 || max_possible_turn_speed_ > 0;
    }
}

#endif
