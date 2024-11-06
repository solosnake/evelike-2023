#include "solosnake/blue/blue_turning_hardpoint.hpp"

namespace blue
{
    inline std::uint16_t Turning_hardpoint::firing_cap_cost() const noexcept
    {
        return shooter_.firing_cap_cost();
    }

    inline std::uint16_t Turning_hardpoint::firing_cooldown() const noexcept
    {
        return shooter_.firing_cooldown();
    }

    inline unsigned int Turning_hardpoint::turning_hardpoint_min_range() const noexcept
    {
        return shooter_.min_range();
    }

    inline unsigned int Turning_hardpoint::turning_hardpoint_max_range() const noexcept
    {
        return shooter_.max_range();
    }

    inline Fixed_angle Turning_hardpoint::turninghardpoint_rotation_speed() const noexcept
    {
        return shooter_.shooter_rotation_speed();
    }

    inline HardpointType Turning_hardpoint::turninghardpoint_type() const noexcept
    {
        return shooter_.hardpoint_type();
    }

    inline Fixed_angle Turning_hardpoint::turninghardpoint_min_angle() const noexcept
    {
        return min_angle_;
    }

    inline Fixed_angle Turning_hardpoint::turninghardpoint_max_angle() const noexcept
    {
        return max_angle_;
    }

    inline Hardpoint_shot Turning_hardpoint::make_a_shot( Randoms& r ) const
    {
        return shooter_.make_a_shot( r );
    }

    inline float Turning_hardpoint::get_falloff_at_range( std::uint16_t range ) const noexcept
    {
        return shooter_.get_shooter_falloff_at_range( range );
    }
}
