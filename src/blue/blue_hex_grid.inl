#include "solosnake/blue/blue_hex_grid.hpp"
#include <cassert>

namespace blue
{
    inline bool operator == (const Hex_grid::Hex_tile& lhs, const Hex_grid::Hex_tile& rhs) noexcept
    {
        return lhs.tile_contents == rhs.tile_contents && lhs.tile_coordinate == rhs.tile_coordinate;
    }

    inline void Hex_grid::calculate_xy( const Hex_coord c, float* unaliased fxy )
    {
        fxy[0] = SS_AXIAL_X_FROM_COORD( c.x, c.y ) * SS_SQRT_3;
        fxy[1] = 2.0f * SS_AXIAL_Y_FROM_COORD( c.x, c.y ) + SS_AXIAL_X_FROM_COORD( c.x, c.y );
    }

    inline constexpr float Hex_grid::tile_inner_radius() noexcept
    {
        // See HexHalfHeight.
        return 1.0f;
    }

    inline HexContentFlags Hex_grid::invalid_tile_value() const noexcept
    {
        return invalid_tile_value_;
    }

    inline unsigned int Hex_grid::max_grid_size() const noexcept
    {
        return width_ * height_;
    }

    inline unsigned int Hex_grid::valid_tiles_count() const noexcept
    {
        return count_valid_tiles();
    }

    inline int Hex_grid::grid_width() const noexcept
    {
        return width_;
    }

    inline int Hex_grid::grid_height() const noexcept
    {
        return height_;
    }

    inline size_t Hex_grid::grid_size() const noexcept
    {
        return grid_.size();
    }

    inline bool Hex_grid::is_in_board_bounds( const Hex_coord c ) const noexcept
    {
        return c.x >= 0 && c.x < width_ && c.y >= 0 && c.y < height_;
    }

    inline HexContentFlags Hex_grid::contents( const Hex_coord c ) const noexcept
    {
        assert( is_in_board_bounds( c ) );
        return grid_[width_ * c.y + c.x];
    }

    inline bool Hex_grid::is_on_board( const Hex_coord xy ) const noexcept
    {
        return is_in_board_bounds( xy ) && ( 0 == ( contents( xy ) & invalid_tile_value_ ) );
    }

    inline void Hex_grid::set_contents( const Hex_coord c, const HexContentFlags flags )
    {
        assert( is_in_board_bounds( c ) );
        grid_.at(width_ * c.y + c.x) = flags;
    }

    inline std::uint16_t Hex_grid::get_step_distance_between( const Hex_coord A, const Hex_coord B )
    {
        // Calculate using axial coordinates.
        const int q1 = SS_AXIAL_X_FROM_COORD( A.x, A.y );
        const int r1 = SS_AXIAL_Y_FROM_COORD( A.x, A.y );
        const int q2 = SS_AXIAL_X_FROM_COORD( B.x, B.y );
        const int r2 = SS_AXIAL_Y_FROM_COORD( B.x, B.y );
        return static_cast<std::uint16_t>(
                   ( std::abs( q1 - q2 ) + std::abs( r1 - r2 ) + std::abs( q1 - q2 + r1 - r2 ) ) / 2 );
    }

    inline int Hex_grid::axial_index( const int x, const int z ) const noexcept
    {
        // Axial back to "odd-q"
        assert( SS_COORD_X_FROM_AXIAL( x, z ) >= 0 );
        assert( SS_COORD_Y_FROM_AXIAL( x, z ) >= 0 );
        assert( SS_COORD_X_FROM_AXIAL( x, z ) < width_ );
        assert( SS_COORD_Y_FROM_AXIAL( x, z ) < height_ );
        return width_ * SS_COORD_Y_FROM_AXIAL( x, z ) + SS_COORD_X_FROM_AXIAL( x, z );
    }
}