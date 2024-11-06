#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_machine_grid_view.hpp"
#include "solosnake/blue/blue_sense_results.hpp"

namespace blue
{
    Sense_results::Sense_results()
        : sense_origin_( Hex_coord::make_coord( Hex_grid::MaxPermittedGridHeight + 1,
                                                Hex_grid::MaxPermittedGridWidth + 1 ) )
    {
    }

    void Sense_results::reserve( const size_t n )
    {
        sensed_tiles_.reserve( n );
        sensed_contents_.reserve( n );
    }

    void Sense_results::sense_surroundings( const Machine_grid_view grid,
                                            const Hex_coord origin,
                                            const std::uint16_t range )
    {
        sensed_tiles_.clear();

        sense_origin_ = origin;

        grid.get_sorted_circle_contents( origin, range, sensed_tiles_,
                                         HEX_FLAGS_BOT | HEX_FLAGS_ASTEROID | HEX_FLAGS_OFF_BOARD );

        sensed_contents_.resize( sensed_tiles_.size(), HexIsUnoccupied );

        for( size_t i = 0; i < sensed_tiles_.size(); ++i )
        {
            auto c = sensed_tiles_[i].tile_contents;
            if( is_bot_on_tile( c ) )
            {
                sensed_contents_[i] = HexHasBot;
            }
            else if( is_asteroid_on_tile( c ) )
            {
                sensed_contents_[i] = HexHasAsteroid;
            }
            else if( is_sun_or_off_board_tile( c ) )
            {
                sensed_contents_[i] = HexIsOffBoard;
            }
        }
    }
}
