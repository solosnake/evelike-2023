#ifndef blue_drawgameboard_hpp
#define blue_drawgameboard_hpp

#include "solosnake/blue/blue_drawboard.hpp"

namespace blue
{
    //! Class that draws a sun tinted 3 color default game board of tiles.
    class drawgameboard : public drawboard
    {
    public:

        drawgameboard(
            solosnake::rendering_system&,
            const solosnake::persistance& settings,
            const solosnake::dynbufferRGBA& sunColour,
            const solosnake::dynbufferRGBA& tileColour0,
            const solosnake::dynbufferRGBA& tileColour1,
            const solosnake::dynbufferRGBA& tileColour2  );

    private:

        void set_board_colours(
            const Hex_grid&,
            const size_t tileCount,
            solosnake::dynbufferRGBA* rgbas ) const override;

    private:

        solosnake::dynbufferRGBA sunColour_;
        solosnake::dynbufferRGBA tilecolours_[3];
    };
}

#endif
