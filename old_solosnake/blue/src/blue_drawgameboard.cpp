#include "solosnake/blue/blue_drawgameboard.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_sunstrength.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/throw.hpp"

using namespace solosnake;

namespace blue
{
    namespace
    {
        drawboardparams load_drawgameboard_settings( solosnake::rendering_system& r,
                                                     const solosnake::persistance& settings )
        {
            drawboardparams params;

            params.transparency_ = settings.value( BLUE_GAME_TILEALPHA ).as_float();
            params.tile_shrink_factor_ = settings.value( BLUE_GAME_TILESHRINKFACTOR ).as_float();
            params.tx_ = settings.value( BLUE_GAME_TILETEXTURE_X ).as_float();
            params.ty_ = settings.value( BLUE_GAME_TILETEXTURE_Y ).as_float();
            params.tw_ = settings.value( BLUE_GAME_TILETEXTURE_XW ).as_float();
            params.th_ = settings.value( BLUE_GAME_TILETEXTURE_YH ).as_float();

            // Check as much as possible before allocating any resources:
            const auto& texname = settings.value( BLUE_GAME_TILETEXTURE ).str();

            if( texname.empty() )
            {
                ss_throw( "No board texture specified." );
            }

            if( params.transparency_ < 0.0f || params.transparency_ > 1.0f )
            {
                ss_throw( BLUE_GAME_TILEALPHA " is out of range." );
            }

            if( params.tile_shrink_factor_ <= 0.0f || params.tile_shrink_factor_ > 1.0f )
            {
                ss_throw( BLUE_GAME_TILESHRINKFACTOR " is out of range." );
            }

            params.texture_ = r.get_cache_ptr()->get_texture( texname );

            return params;
        }
    }

    drawgameboard::drawgameboard( solosnake::rendering_system& rs,
                                  const solosnake::persistance& settings,
                                  const solosnake::dynbufferRGBA& sunColour,
                                  const solosnake::dynbufferRGBA& tileColour0,
                                  const solosnake::dynbufferRGBA& tileColour1,
                                  const solosnake::dynbufferRGBA& tileColour2 )
        : drawboard( rs, load_drawgameboard_settings( rs, settings ) )
    {
        sunColour_      = sunColour;
        tilecolours_[0] = tileColour0;
        tilecolours_[1] = tileColour1;
        tilecolours_[2] = tileColour2;
    }

    void drawgameboard::set_board_colours( const hexgrid& g,
                                           const size_t tileCount,
                                           solosnake::dynbufferRGBA* clr ) const
    {
        const solosnake::dynbufferRGBA colors[2][3] =
        {
            { tilecolours_[0], tilecolours_[1], tilecolours_[2] },
            { tilecolours_[2], tilecolours_[0], tilecolours_[1] }
        };

        auto dst = clr;

        const auto h = g.grid_height();
        const auto w = g.grid_width();

        for( int8_t y = 0; y < h; ++y )
        {
            for( int8_t x = 0; x < w; ++x )
            {
                const Hex_coord xy = Hex_coord::make_coord( x, y );
                const HexContentFlags content = g.contents( xy );
                const bool draw = is_not_offboard_tile( content );

                if( draw )
                {
                    solosnake::dynbufferRGBA tileColor = colors[x % 2][y % 3];

                    if( sun_strength( content ) > 0 )
                    {
                        float f = sun_strength( content ) / ( 1.0f + static_cast<float>( SunStrength7 ) );
                        tileColor = lerp( tileColor, sunColour_, f );
                    }

                    dst[0] = tileColor;
                    dst[1] = tileColor;
                    dst[2] = tileColor;
                    dst[3] = tileColor;
                    dst[4] = tileColor;
                    dst[5] = tileColor;

                    dst += 6;
                }

                assert( ( static_cast<size_t>( dst - clr ) / 6u ) <= tileCount );
            }
        }
    }
}
