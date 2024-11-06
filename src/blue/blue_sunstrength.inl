#include "solosnake/blue/blue_sunstrength.hpp"

namespace blue
{
    inline constexpr SunStrength max_sun_strength() noexcept
    {
        return SunStrength7;
    }

    /// Returns true if @a s is in the range to be a value SunStrength enum.
    inline constexpr bool can_be_a_sun_strength( const std::uint16_t s ) noexcept
    {
        static_assert( SunStrength0 == 0, "Lower bound (SunStrength0) not zero." );
        return s <= SunStrength7;
    }
}