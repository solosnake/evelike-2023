#ifndef blue_pickresult_hpp
#define blue_pickresult_hpp

#include "solosnake/point.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"

namespace blue
{
    //! Returns the result of a 'pick' on the board: a screen click that
    //! intersects the board plane. If the pick touched a hexagon tile that
    //! is on the board, then onboard is true. If the pick touched a tile
    //! that is off board (e.g. a Sun or invalid) then onboard is false.
    //! If the pick touched a location on the board plane that lies within
    //! the grid boundary (the width x height surface of the board, whether or
    //! not the tiles are valid or not) then the boardcoord is always valid.
    struct pickresult
    {
        //! By default constructs a completely invalid off board pickresult.
        pickresult();

        bool                onboard;            // True if pick landed on valid tile
        solosnake::point3d  worldPoint;
        HexContentFlags     location_contents;  // Might be invalid.
        Hex_coord           boardcoord;         // Valid if within board bounds (width x height).
    };
}

#endif
