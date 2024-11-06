#ifndef blue_drawboard_hpp
#define blue_drawboard_hpp

#include <memory>
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/persistance.hpp"

namespace blue
{
    class Hex_grid;
    class datapaths;

    //! Drawboard settings.
    struct drawboardparams
    {
        solosnake::texturehandle_t texture_;
        float                      transparency_;
        float                      tile_shrink_factor_;
        float                      tx_;
        float                      ty_;
        float                      tw_;
        float                      th_;
    };

    //! Contains an up-to-date model of the boards polygons, tracking different
    //! states of the board. This owns the polygons of the board, and is shared
    //! by different views.
    //! This class owns the board Y and the hexagon shrink factor, which are
    //! purely visual settings.
    //! This is a very lightweight class with no ownership requirements really.
    //! This does not draw the asteroids or suns or machines etc on the board.
    //! This literally only draws the hexagon board.
    class drawboard
    {
    public:

        drawboard( solosnake::rendering_system&, const drawboardparams& );

        virtual ~drawboard();

        //! Note: Rebuilding the board each frame seems to be faster, probably
        //!       because overwriting a subset of a named buffer requires a lock
        //!       on the GPU. Fix this by putting color into its own named buffer.
        //! @onlyValidTiles Only storage for valid tiles is allocated when true,
        //! else storage for the entire grid (width x height) is allocated.
        void make_board_buffer( const Hex_grid&, const bool onlyValidTiles );

        void release_board_buffer();

        //! Forces a re-coloration of the board hexagon tiles. This is done
        //! by the set_board_colours virtual function.
        void update_board_colours( const Hex_grid& );

        void render() const;

    private:

        //! Override this to custom color the board. rgbas is an array of dynbufferRGBA
        //! objects, 6 per tileCount (one per vertex). The hexgrid is the same grid passed
        //! to make_board_buffer and update_board_colours.
        virtual void set_board_colours(
            const Hex_grid&,
            const size_t tileCount,
            solosnake::dynbufferRGBA* rgbas ) const = 0;

    private:

        std::weak_ptr<solosnake::deferred_renderer>         renderer_;
        solosnake::deferred_renderer::dynbufferdrawparams   drawbuffer_;
        drawboardparams                                     params_;
    };

}

#endif
