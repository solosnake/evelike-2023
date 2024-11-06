#include <algorithm>
#include <memory>
#include <climits>
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_half_line_3d.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_line_hit.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Game_hex_grid::Game_hex_grid()
    : grid_()
    {
        // NOP
    }

    Game_hex_grid::Game_hex_grid( Hex_grid&& h ) : grid_( std::move( h ) )
    {
        if( grid_.number_of_islands() != 1 )
        {
            ss_throw( "game board must contain exactly one island of tiles." );
        }
    }

    bool Game_hex_grid::is_tile_reserved( const Hex_coord xy ) const noexcept
    {
        return grid_.is_on_board( xy ) && is_reserved_tile( grid_.contents( xy ) );
    }

    bool Game_hex_grid::is_tile_empty_and_unreserved( Hex_coord xy ) const noexcept
    {
        return grid_.is_on_board( xy ) && is_empty_and_unreserved_tile( grid_.contents( xy ) );
    }

    void Game_hex_grid::place_asteroid_on_tile( const Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_not_off_board_tile( grid_.contents( xy ) ) );
        assert( is_empty_and_unreserved_tile( grid_.contents( xy ) ) );

        if( ! is_on_board( xy ) )
        {
            ss_throw( "Asteroid coord in not on board." );
        }

        if( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) )
        {
            ss_throw( "Asteroid was placed on occupied tile." );
        }

        grid_.set_contents( xy, asteroid_tile_value() | sun_flag_contents( xy ) );

        assert( is_asteroid_on_tile( grid_.contents( xy ) ) );
        assert( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) );
    }

    void Game_hex_grid::place_sun_on_tile( SunStrength strength, const Hex_coord xy )
    {
        assert( grid_.is_in_board_bounds( xy ) );
        assert( is_off_board_tile( grid_.contents( xy ) ) || is_empty_and_unreserved_tile( grid_.contents( xy ) ) );

        if( ! grid_.is_in_board_bounds( xy ) )
        {
            ss_throw( "Sun coord in not on board." );
        }

        if( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) )
        {
            if( ! is_off_board_tile( grid_.contents( xy ) ) )
            {
                ss_throw( "Sun was placed on occupied tile." );
            }
        }

        grid_.set_contents( xy, off_board_tile_value() );

        // Find all the tiles in the range of this Sun and add to them
        // this Sun's strength, up to a max of SunStrength7.
        std::vector<Hex_grid::Hex_tile> tiles;
        get_sorted_circle_contents( xy, strength, tiles );

        for( size_t i = 1; i < tiles.size(); ++i )
        {
            unsigned int step_distance = Hex_grid::get_step_distance_between( tiles[i].tile_coordinate, xy );
            HexContentFlags content    = grid_.contents( tiles[i].tile_coordinate );
            unsigned int ss            = std::min<unsigned int>( SunStrength7, 1 + sun_strength( content ) + strength - step_distance );
            HexContentFlags sun_flags  = sun_strength_as_flags( static_cast<std::uint8_t>( ss ) );
            grid_.set_contents( tiles[i].tile_coordinate, sun_flags | ( content & tile_value_mask() ) );
        }

        assert( is_sun_or_off_board_tile( grid_.contents( xy ) ) );
        assert( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) );
        assert( ! is_on_board( xy ) );
    }

    void Game_hex_grid::remove_asteroid_from_tile( const Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_not_off_board_tile( grid_.contents( xy ) ) );
        assert( is_asteroid_on_tile( grid_.contents( xy ) ) );
        assert( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) );

        grid_.set_contents( xy, empty_tile_value() | sun_flag_contents( xy ) );

        assert( is_empty_and_unreserved_tile( grid_.contents( xy ) ) );
        assert( is_on_board( xy ) );
        assert( ! is_asteroid_on_tile( grid_.contents( xy ) ) );
        assert( is_empty_and_unreserved_tile( grid_.contents( xy ) ) );
    }

    void Game_hex_grid::place_bot_on_tile( Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_not_off_board_tile( grid_.contents( xy ) ) );
        assert( is_empty_and_unreserved_tile( grid_.contents( xy ) ) );

        grid_.set_contents( xy, bot_tile_value() | sun_flag_contents( xy ) );

        assert( ! is_empty_and_unreserved_tile( grid_.contents( xy ) ) );
        assert( is_bot_on_tile( grid_.contents( xy ) ) );
        assert( is_on_board( xy ) );
    }

    void Game_hex_grid::move_bot_from_tile_to_tile( Hex_coord from, Hex_coord to )
    {
        assert( is_bot_on_tile( grid_.contents_at( from ) ) );
        assert( ! is_bot_on_tile( grid_.contents_at( to ) ) );
        assert( is_on_board( from ) );
        assert( is_on_board( to ) );
        assert( is_not_off_board_tile( grid_.contents_at( from ) ) );
        assert( is_not_off_board_tile( grid_.contents_at( to ) ) );
        assert( is_reserved_tile( grid_.contents_at( to ) ) );

        grid_.set_contents( from, empty_tile_value() | sun_flag_contents( from ) );
        grid_.set_contents( to,   bot_tile_value()   | sun_flag_contents( to ) );

        assert( is_on_board( from ) );
        assert( is_on_board( to ) );
        assert( is_empty_and_unreserved_tile( grid_.contents_at( from ) ) );
        assert( is_bot_on_tile( grid_.contents_at( to ) ) );
        assert( ! is_bot_on_tile( grid_.contents_at( from ) ) );
    }

    Hex_grid::Hex_tile Game_hex_grid::get_line_intersection( const Half_line_3d& line ) const
    {
        Hex_grid::Hex_tile result;
        result.tile_contents   = invalid_tile_value();
        result.tile_coordinate = Hex_coord::from_uint16( USHRT_MAX );

        constexpr float board_y = 0.0f;
        float intersectionXYZ[3];
        if( find_half_line_y_plane_intersection( line, board_y, intersectionXYZ ) )
        {
            ss_log( "World ", intersectionXYZ[0], " ", intersectionXYZ[1], " ", intersectionXYZ[2] );
            Hex_coord coords = Hex_grid::calculate_coord( intersectionXYZ[0], intersectionXYZ[2] );

            if( grid_.is_in_board_bounds( coords ) )
            {
                result.tile_contents   = grid_.contents_at( coords );
                result.tile_coordinate = coords;
            }
        }

        return result;
    }

    void Game_hex_grid::contents_under_line( const Hex_coord A, const Hex_coord B, const std::uint16_t maxSteps, std::vector<Hex_coord>& v ) const
    {
        grid_.contents_under_line( A, B, maxSteps, HEX_FLAGS_BOT | HEX_FLAGS_ASTEROID | HEX_FLAGS_SUN, v );
    }

    bool Game_hex_grid::operator == (const Game_hex_grid& rhs) const noexcept
    {
        return this->grid_ == rhs.grid_;
    }

    bool Game_hex_grid::operator != (const Game_hex_grid& rhs) const noexcept
    {
        return this->grid_ != rhs.grid_;
    }
}
