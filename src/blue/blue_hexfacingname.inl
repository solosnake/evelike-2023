#include "solosnake/blue/blue_hexfacingname.hpp"

namespace blue
{
    inline HexFacingName operator + (HexFacingName x, std::int32_t turns) noexcept
    {
        return static_cast<HexFacingName>( (int{x} + (turns % 6) ) % 6 );
    }
}
