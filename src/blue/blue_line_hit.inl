#include "solosnake/blue/blue_line_hit.hpp"

namespace blue
{
    inline Line_hit::Line_hit(HexContentFlags f, Hex_coord c) noexcept
        : hit(true)
        , content(f)
        , xy(c)
    {
    }
}