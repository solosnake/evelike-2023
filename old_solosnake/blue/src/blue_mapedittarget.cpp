#include "solosnake/blue/blue_mapedittarget.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"

namespace blue
{
    mapedittarget::mapedittarget() 
        : grid_( hexgrid::MaxPermittedGridWidth,
                 hexgrid::MaxPermittedGridHeight,
                 empty_tile_value(),
                 offboard_tile_value() )
    {
    }
}
