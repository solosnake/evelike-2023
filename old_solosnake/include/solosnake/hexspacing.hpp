#ifndef solosnake_hexspacing_hpp
#define solosnake_hexspacing_hpp

#include "solosnake/external/lua.hpp"

namespace solosnake
{
    //! Add to Lua the hex spacing helper function
    //! 'hexspacing(x, y, size, spacing, direction)' which returns the
    //! coordinates for a hexing oriented at between 0 (top) to 5 (top
    //! left) clockwise around the hexagon x,y.
    void expose_hexspacing_function( lua_State* );

    //! Returns in @a result the top left coordinates for a neighbour of the
    //! hexagon with top left at @a xy, oriented at @a direction clockwise from
    //! the top, and with a gap between the hexagons of @a spacing.
    //! Both hexagons are oriented horizontally, with a flat top, and both
    //! fit inside a square of side-length @a size.
    void hexspacing( const float xy[2], 
                     float size, 
                     float spacing, 
                     unsigned int direction, 
                     float result[2] );
}

#endif
