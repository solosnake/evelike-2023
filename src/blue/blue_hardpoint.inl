#include "solosnake/blue/blue_hardpoint.hpp"

namespace blue
{
    inline const Hardpoint_shooter& Hardpoint::shooter() const noexcept
    {
        return shooter_;
    }

    inline const Component& Hardpoint::details() const noexcept
    {
        return component_;
    }

    inline Fixed_angle Hardpoint::get_shooter_mid_angle() const noexcept
    {
        return shooter_.get_mid_angle();
    }
}
