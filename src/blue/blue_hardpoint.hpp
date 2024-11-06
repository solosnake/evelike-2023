#ifndef blue_hardpoint_hpp
#define blue_hardpoint_hpp

#include <cstdint>
#include <string>
#include <vector>
#include "solosnake/blue/blue_component.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint_shooter.hpp"

namespace blue
{
    enum ComponentCategory : std::int32_t;
    enum HardpointType : std::uint8_t;

    /// A Hardpoint is a functional Component of a Machine - for example a
    /// weapon system. It does not modify the Hardware platform in any way
    /// except perhaps as a cost.
    ///
    /// "A Hardpoint, or weapon station, is any part of an airframe designed to
    /// carry an external load."
    class Hardpoint final
    {
    public:

        Hardpoint(const Component&, const Hardpoint_shooter&);

        const Hardpoint_shooter& shooter() const noexcept;

        const Component& details() const noexcept;

        bool operator == (const Hardpoint& ) const noexcept;

        bool operator != (const Hardpoint& ) const noexcept;

        /// Returns the (Fixed_angle) angle of the shooter, midway between
        /// its min and max angles.
        Fixed_angle get_shooter_mid_angle() const noexcept;

    private:

        Component         component_;
        Hardpoint_shooter shooter_;
    };

    /// Returns true only if the Hardpoint type can be placed into this
    /// category.
    bool is_validate_categorisation( ComponentCategory c, HardpointType h );
}

#include "solosnake/blue/blue_hardpoint.inl"
#endif
