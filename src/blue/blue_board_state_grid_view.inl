#include "solosnake/blue/blue_board_state_grid_view.hpp"
#include <cassert>

namespace blue
{
    inline size_t Board_state_grid_view::machines_count() const
    {
        return machines_.size();
    }

    inline Machine* Board_state_grid_view::operator[]( size_t i )
    {
        assert( i < machines_.size() );
        return machines_[i].get();
    }

    inline const Machine* Board_state_grid_view::operator[]( size_t i ) const
    {
        assert( i < machines_.size() );
        return machines_[i].get();
    }

    inline HexContentFlags Board_state_grid_view::contents( Hex_coord xy ) const
    {
        return grid_.contents( xy );
    }

    inline void Board_state_grid_view::contents_under_line( const Hex_coord A,
            const Hex_coord B,
            const std::uint16_t maxSteps,
            std::vector<Hex_coord>& v ) const
    {
        grid_.contents_under_line( A, B, maxSteps, v );
    }

    inline void Board_state_grid_view::get_sorted_circle_contents( const Hex_coord xy,
            const std::uint16_t r,
            std::vector<Hex_grid::Hex_tile>& tiles,
            HexContentFlags flags ) const
    {
        grid_.get_sorted_circle_contents( xy, r, tiles, flags );
    }

    inline bool Board_state_grid_view::try_get_neighbour_tile( const Hex_coord xy,
            const HexFacingName facing,
            Hex_coord& c ) const
    {
        return grid_.try_get_neighbour_tile( xy, facing, c );
    }

    inline unsigned int Board_state_grid_view::grid_width() const noexcept
    {
        return grid_.grid_width();
    }

    inline unsigned int Board_state_grid_view::grid_height() const noexcept
    {
        return grid_.grid_height();
    }

    inline bool Board_state_grid_view::is_tile_reserved( const Hex_coord xy ) const noexcept
    {
        return grid_.is_tile_reserved( xy );
    }

    inline bool Board_state_grid_view::is_tile_empty_and_unreserved( const Hex_coord xy ) const noexcept
    {
        return grid_.is_tile_empty_and_unreserved( xy );
    }

    inline bool Board_state_grid_view::is_on_board( const Hex_coord xy ) const noexcept
    {
        return grid_.is_on_board( xy );
    }

    inline void Board_state_grid_view::unreserve_empty_tile( const Hex_coord xy )
    {
        grid_.unreserve_empty_tile( xy );
    }

    inline void Board_state_grid_view::reserve_empty_tile( const Hex_coord xy )
    {
        grid_.reserve_empty_tile( xy );
    }

    inline void Board_state_grid_view::place_asteroid_on_tile( const Hex_coord xy )
    {
        grid_.place_asteroid_on_tile( xy );
    }

    inline void Board_state_grid_view::remove_asteroid_from_tile( const Hex_coord xy )
    {
        grid_.remove_asteroid_from_tile( xy );
    }

    inline void Board_state_grid_view::place_sun_on_tile( SunStrength strength, const Hex_coord xy )
    {
        grid_.place_sun_on_tile( strength, xy );
    }

    inline const Game_hex_grid& Board_state_grid_view::hex_board() const noexcept
    {
        return grid_;
    }

    inline const Hex_grid& Board_state_grid_view::grid() const noexcept
    {
        return grid_.grid();
    }
}
