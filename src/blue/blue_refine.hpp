#ifndef blue_refine_hpp
#define blue_refine_hpp

#include <cstdint>

namespace blue
{
    class Amount;
    enum OreTypes : std::uint8_t;

    /// Returns the amount of metals @a n units of @a ore refines to.
    Amount refine( OreTypes ore, std::uint16_t n ) noexcept;

    /// How much harder a given ore is to refine.
    float refine_period_factor( OreTypes ore ) noexcept;
}

#endif
