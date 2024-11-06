#include "solosnake/blue/blue_game_hex_grid.hpp"
#include <cassert>

namespace blue
{
    inline const Hex_grid& Game_hex_grid::grid() const noexcept
    {
        return grid_;
    }

    inline HexContentFlags Game_hex_grid::invalid_tile_value() const noexcept
    {
        return grid_.invalid_tile_value();
    }

    inline HexContentFlags Game_hex_grid::contents( const Hex_coord xy ) const
    {
        return grid_.is_in_board_bounds( xy )
             ? grid_.contents_at( xy )
             : off_board_tile_value();
    }

    inline HexContentFlags Game_hex_grid::get_neighbour_contents( const Hex_coord xy,
                                                                  const HexFacingName facing ) const
    {
        return grid_.get_neighbour_contents( xy, facing );
    }

    inline SunStrength Game_hex_grid::tile_sun_strength( Hex_coord xy ) const
    {
        return static_cast<SunStrength>( sun_strength( grid_.contents_at( xy ) ) );
    }

    inline unsigned int Game_hex_grid::grid_width() const noexcept
    {
        return grid_.grid_width();
    }

    inline unsigned int Game_hex_grid::grid_height() const noexcept
    {
        return grid_.grid_height();
    }

    inline bool Game_hex_grid::is_on_board( const Hex_coord xy ) const noexcept
    {
        return grid_.is_on_board( xy );
    }

    inline bool Game_hex_grid::try_get_neighbour_tile( const Hex_coord xy,
                                                       const HexFacingName facing,
                                                       Hex_coord& c ) const noexcept
    {
        return grid_.try_get_neighbour_tile( xy, facing, c );
    }

    inline void Game_hex_grid::get_sorted_circle_contents( const Hex_coord xy,
                                                           const std::uint16_t r,
                                                           std::vector<Hex_grid::Hex_tile>& v,
                                                           HexContentFlags flags ) const
    {
        grid_.get_sorted_circle_contents( xy, r, v, flags );
    }

    inline std::uint16_t Game_hex_grid::get_step_distance_between( const Hex_coord A,
                                                                   const Hex_coord B ) const
    {
        return Hex_grid::get_step_distance_between( A, B );
    }

    inline Rectf Game_hex_grid::surface_rect() const noexcept
    {
        return grid_.surface_rect();
    }

    inline void Game_hex_grid::remove_bot_from_tile( Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_bot_on_tile( grid_.contents_at( xy ) ) );

        grid_.set_contents( xy, empty_tile_value() | sun_flag_contents( xy ) );

        assert( is_on_board( xy ) );
    }

    inline void Game_hex_grid::reserve_empty_tile( Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_empty_and_unreserved_tile( grid_.contents_at( xy ) ) );

        grid_.set_contents( xy, reserved_tile_value() | sun_flag_contents( xy ) );

        assert( is_reserved_tile( grid_.contents_at( xy ) ) );
        assert( is_on_board( xy ) );
    }

    inline void Game_hex_grid::unreserve_empty_tile( Hex_coord xy )
    {
        assert( is_on_board( xy ) );
        assert( is_reserved_tile( grid_.contents_at( xy ) ) );

        grid_.set_contents( xy, empty_tile_value() | sun_flag_contents( xy ) );

        assert( is_empty_and_unreserved_tile( grid_.contents_at( xy ) ) );
        assert( is_on_board( xy ) );
    }

    inline HexContentFlags Game_hex_grid::sun_flag_contents( Hex_coord xy ) const
    {
        return contents( xy ) & sun_strength_mask();
    }
}
