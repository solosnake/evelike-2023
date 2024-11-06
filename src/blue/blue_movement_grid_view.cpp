#include "solosnake/blue/blue_movement_grid_view.hpp"
#include "solosnake/blue/blue_board_state_grid_view.hpp"

namespace blue
{
    Movement_grid_view::Movement_grid_view( Board_state_grid_view& g ) : grid_( &g )
    {
    }

    bool Movement_grid_view::is_on_board( const Hex_coord xy ) const
    {
        return grid_->is_on_board( xy );
    }

    bool Movement_grid_view::is_tile_reserved( const Hex_coord xy ) const
    {
        return grid_->is_tile_reserved( xy );
    }

    bool Movement_grid_view::is_tile_empty_and_unreserved( const Hex_coord xy ) const
    {
        return grid_->is_tile_empty_and_unreserved( xy );
    }

    bool Movement_grid_view::try_get_neighbour_tile(
        const Hex_coord xy,
        const HexFacingName facing,
        Hex_coord& c ) const
    {
        return grid_->try_get_neighbour_tile( xy, facing, c );
    }

    void Movement_grid_view::move_bot_from_tile_to_tile( const Hex_coord from, const Hex_coord to )
    {
        return grid_->move_bot_from_tile_to_tile( from, to );
    }

    void Movement_grid_view::reserve_empty_tile( const Hex_coord xy )
    {
        return grid_->reserve_empty_tile( xy );
    }
}
