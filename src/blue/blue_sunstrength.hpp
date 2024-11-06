#ifndef blue_sunstrength_hpp
#define blue_sunstrength_hpp

#include <cstdint>

namespace blue
{
    /// Representation of how strong a nearby Sun's effect is on a tile.
    /// The stronger the Sun the more solar energy a tile yields per frame.
    enum SunStrength : std::uint16_t
    {
        SunStrength0,
        SunStrength1,
        SunStrength2,
        SunStrength3,
        SunStrength4,
        SunStrength5,
        SunStrength6,
        SunStrength7
    };

    inline constexpr SunStrength max_sun_strength() noexcept;

    /// Returns true if @a s is in the range to be a value SunStrength enum.
    inline constexpr bool can_be_a_sun_strength( const std::uint16_t s ) noexcept;
}

#include "solosnake/blue/blue_sunstrength.inl"
#endif
