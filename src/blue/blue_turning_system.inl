#include "solosnake/blue/blue_turning_system.hpp"

namespace blue
{
    inline bool Turning_system::is_turning() const noexcept
    {
        return dAdt_ != 0;
    }

    inline bool Turning_system::is_not_turning() const noexcept
    {
        return dAdt_ == 0;
    }

    inline bool Turning_system::is_turning_cw() const noexcept
    {
        return dAdt_ > 0;
    }

    inline bool Turning_system::is_turning_ccw() const noexcept
    {
        return dAdt_ < 0;
    }

    inline Fixed_angle Turning_system::angle() const noexcept
    {
        return angle_;
    }

    inline HexFacingName Turning_system::get_facing() const
    {
        return blue::facing_from_fixedAngle(angle_);
    }

    inline HexFacingName Turning_system::facing_wanted() const noexcept
    {
        return static_cast<HexFacingName>(facingWanted_);
    }

    inline int Turning_system::max_turn_speed() const noexcept
    {
        return max_turn_speed_;
    }

    inline float Turning_system::turn_speed_level() const noexcept
    {
        return max_turn_speed_
                   ? static_cast<float>(std::abs(dAdt_)) / max_turn_speed_
                   : 0.0f;
    }
}
