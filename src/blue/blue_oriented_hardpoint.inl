#include "solosnake/blue/blue_oriented_hardpoint.hpp"

namespace blue
{
    inline ComponentOrientation Oriented_hardpoint::hardpoint_orientation() const noexcept
    {
        return orientation_;
    }

    inline const Hardpoint& Oriented_hardpoint::oriented_hardpoint() const noexcept
    {
        return hardpoint_;
    }
}
