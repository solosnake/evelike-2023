#ifndef blue_turning_hardpoint_hpp
#define blue_turning_hardpoint_hpp

#include <cstdint>
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint_shooter.hpp"

namespace blue
{
    class Hardpoint;
    class Randoms;


    class Turning_hardpoint
    {
    public:

        Turning_hardpoint(
            const size_t indexOnMachine,
            const Fixed_angle angleOnMachine,
            const Hardpoint& );

        std::uint16_t firing_cap_cost() const noexcept;

        std::uint16_t firing_cooldown() const noexcept;

        unsigned int  turning_hardpoint_min_range() const noexcept;

        unsigned int  turning_hardpoint_max_range() const noexcept;

        Fixed_angle  turninghardpoint_rotation_speed() const noexcept;

        HardpointType turninghardpoint_type() const noexcept;

        Fixed_angle  turninghardpoint_min_angle() const noexcept;

        Fixed_angle  turninghardpoint_max_angle() const noexcept;

        Hardpoint_shot make_a_shot( Randoms& r ) const;

        float get_falloff_at_range( std::uint16_t range ) const noexcept;

        Hardpoint_shooter   shooter_;           // The immutable characteristics of this hardpoint.
        Fixed_angle         current_angle_;     // Angle relative to Machine (in plane of Machine).
        Fixed_angle         min_angle_;         // Angle relative to Machine.
        Fixed_angle         max_angle_;         // Angle relative to Machine.
        std::uint32_t       cooldown_state_;    // Zero when ready to fire again.
    };
}

#include "solosnake/blue/blue_turning_hardpoint.inl"
#endif
