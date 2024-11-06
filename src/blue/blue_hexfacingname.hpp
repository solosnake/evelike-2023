#ifndef blue_hexfacingname_hpp
#define blue_hexfacingname_hpp

#include <cstdint>

namespace blue
{
    // The Neighbours are named as follows, with coords for illustration of axis //
    //  directions:                                                              //
    //                                                                           //
    //                     _ _ _ _ _                                             //
    //                    /         \                                            //
    //                   /           \                                           //
    //         _ _ _ _ _/    Tile0    \_ _ _ _ _                                 //
    //        /         \     1,0     /         \                                //
    //       /           \           /           \                               //
    //      /    Tile5    \_ _ _ _ _/    Tile1    \                              //
    //      \     0,1     /         \     2,0     /                              //
    //       \           /           \           /                               //
    //        \_ _ _ _ _/     1,1     \_ _ _ _ _/                                //
    //        /         \             /         \                                //
    //       /           \           /           \                               //
    //      /    Tile4    \_ _ _ _ _/    Tile2    \                              //
    //      \     0,2     /         \     2,1     /                              //
    //       \           /           \           /                               //
    //        \_ _ _ _ _/    Tile3    \_ _ _ _ _/                                //
    //                  \     1,2     /                                          //
    //                   \           /                                           //
    //                    \_ _ _ _ _/                                            //
    //                                                                           //
    //                                                                           //
    //                                                                           //

    /// Standardised names for the Neighbours of a given hex.                    //
    enum HexFacingName : std::int16_t
    {
        FacingTile0 = 0,
        FacingTile1 = 1,
        FacingTile2 = 2,
        FacingTile3 = 3,
        FacingTile4 = 4,
        FacingTile5 = 5
    };

    /// Returns the HexFacingName after @a turns clockwise starting from
    /// @a x. Zero turns will return x etc.
    HexFacingName operator + (HexFacingName x, std::int32_t turns) noexcept;
}

#include "solosnake/blue/blue_hexfacingname.inl"
#endif
