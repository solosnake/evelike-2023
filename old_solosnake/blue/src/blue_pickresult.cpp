#include "solosnake/blue/blue_pickresult.hpp"

namespace blue
{
    pickresult::pickresult()
        : onboard( false )
        , worldPoint()
        , location_contents( offboard_tile_value() )
        , boardcoord( Hex_coord::make_invalid_coord() )
    {
    }
}
