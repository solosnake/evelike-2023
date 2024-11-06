#include "solosnake/blue/blue_oriented_hardpoint.hpp"

namespace blue
{
    Oriented_hardpoint::Oriented_hardpoint(ComponentOrientation dir, const Hardpoint& h )
    : hardpoint_(h)
    , orientation_(dir)
    {
    }

    bool Oriented_hardpoint::operator == (const Oriented_hardpoint& rhs) const noexcept
    {
        return (hardpoint_   == rhs.hardpoint_)
            && (orientation_ == rhs.orientation_)
            ;
    }

    bool Oriented_hardpoint::operator != (const Oriented_hardpoint& rhs) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
