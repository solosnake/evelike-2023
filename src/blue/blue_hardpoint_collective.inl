#include "solosnake/blue/blue_hardpoint_collective.hpp"

namespace blue
{
    inline std::uint16_t Hardpoint_collective::max_range() const noexcept
    {
        return max_range_;
    }

    inline std::uint16_t Hardpoint_collective::min_range() const noexcept
    {
        return min_range_;
    }

    inline void Hardpoint_collective::change_location( const Hex_coord xy ) noexcept
    {
        aiming_ = false;
        machine_location_ = xy;
    }

    inline void Hardpoint_collective::change_orientation( const Fixed_angle angle ) noexcept
    {
        aiming_ = false;
        machine_facing_angle_ = angle;
    }

    inline void Hardpoint_collective::tick()
    {
        if( cooling_ )
        {
            tick_cooldown();
        }

        if( aiming_ )
        {
            tick_aim();
        }
    }

    inline float Hardpoint_collective::get_indexed_hardpoint_falloff_at_range( const size_t i,
                                                                               const std::uint16_t range ) const
    {
        assert( i < hardpoints_.size() );
        assert( hardpoints_.at(i).get_falloff_at_range( range ) >= 0.0f );
        return hardpoints_.at(i).get_falloff_at_range( range );
    }

    inline const std::vector<Turning_hardpoint>& Hardpoint_collective::turning_hardpoints() const noexcept
    {
        return hardpoints_;
    }
}