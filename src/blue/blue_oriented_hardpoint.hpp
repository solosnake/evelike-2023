#ifndef blue_oriented_hardpoint_hpp
#define blue_oriented_hardpoint_hpp

#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_componentorientation.hpp"

namespace blue
{
    class Oriented_hardpoint
    {
    public:

        Oriented_hardpoint( ComponentOrientation dir, const Hardpoint& h );

        ComponentOrientation hardpoint_orientation() const noexcept;

        const Hardpoint&     oriented_hardpoint() const noexcept;

        bool operator == (const Oriented_hardpoint& rhs) const noexcept;
        bool operator != (const Oriented_hardpoint& rhs) const noexcept;

    private:

        Hardpoint            hardpoint_;
        ComponentOrientation orientation_;
    };
}

#include "solosnake/blue/blue_oriented_hardpoint.inl"
#endif
