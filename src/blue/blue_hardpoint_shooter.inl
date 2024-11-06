#include "solosnake/blue/blue_hardpoint_shooter.hpp"

namespace blue
{
    inline std::uint16_t Hardpoint_shooter::firing_cap_cost() const noexcept
    {
        return firing_cap_cost_;
    }

    inline std::uint16_t Hardpoint_shooter::firing_cooldown() const noexcept
    {
        return firing_cooldown_;
    }

    inline std::uint16_t Hardpoint_shooter::min_range() const noexcept
    {
        return min_range_;
    }

    inline std::uint16_t Hardpoint_shooter::max_range() const noexcept
    {
        return hardpoint_shot_.max_range_;
    }

    inline Fixed_angle Hardpoint_shooter::min_angle() const noexcept
    {
        return degrees_to_fixed_angle( min_angle_degrees_ );
    }

    inline Fixed_angle Hardpoint_shooter::max_angle() const noexcept
    {
        return degrees_to_fixed_angle( max_angle_degrees_ );
    }

    inline Fixed_angle Hardpoint_shooter::shooter_rotation_speed() const noexcept
    {
        return rotation_speed_;
    }

    inline HardpointType Hardpoint_shooter::hardpoint_type() const noexcept
    {
        return hardpoint_shot_.hardpoint_type_;
    }

    inline float Hardpoint_shooter::get_shooter_falloff_at_range( const std::uint16_t range ) const noexcept
    {
        return Hardpoint_shooter::calculate_falloff( range,
                                                     min_range_,
                                                     hardpoint_shot_.max_range_,
                                                     max_range_percent_effect_ );
    }
}
