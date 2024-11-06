#ifndef blue_hexsenseresult_hpp
#define blue_hexsenseresult_hpp

#include <cstdint>
#include "solosnake/blue/blue_hexcontentflags.hpp"

namespace blue
{
    /// Results returned about a hex. These values are available to the Machine
    /// Instructions and are returned by the 'get_sense_result_type' command.
    enum HexSenseResult : std::uint16_t
    {
        HexIsUnoccupied,
        HexHasBot,
        HexHasAsteroid,
        HexIsOffBoard
    };
}

#endif
