#ifndef blue_line_hit_hpp
#define blue_line_hit_hpp

#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"

namespace blue
{
    /// Struct-like class, contents are valid only if 'hit' is true.
    class Line_hit
    {
    public:

        Line_hit() = default;

        Line_hit(HexContentFlags f, Hex_coord c) noexcept;

        bool            hit{false};
        HexContentFlags content{0u};
        Hex_coord       xy{};
    };

    static_assert(sizeof(Line_hit) == 4, "Size of Line_hit is not what was expected.");
}

#include "solosnake/blue/blue_line_hit.inl"
#endif
