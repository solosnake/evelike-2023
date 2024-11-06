#include "solosnake/blue/blue_machine_grid_view.hpp"

namespace blue
{
    inline Machine_grid_view::Machine_grid_view(Game_hex_grid &g) : grid_(&g)
    {
    }

    inline void Machine_grid_view::reserve_empty_tile(const Hex_coord xy)
    {
        return grid_->reserve_empty_tile(xy);
    }

    inline HexContentFlags Machine_grid_view::contents(const Hex_coord xy) const
    {
        return grid_->contents(xy);
    }

    inline size_t Machine_grid_view::get_step_distance_between(
        const Hex_coord A,
        const Hex_coord B) const
    {
        return grid_->get_step_distance_between(A, B);
    }

    inline bool Machine_grid_view::is_on_board(const Hex_coord xy) const
    {
        return grid_->is_on_board(xy);
    }

    inline bool Machine_grid_view::is_tile_empty_and_unreserved(const Hex_coord xy) const
    {
        return grid_->is_tile_empty_and_unreserved(xy);
    }

    inline SunStrength Machine_grid_view::tile_sun_strength(Hex_coord xy) const
    {
        return grid_->tile_sun_strength(xy);
    }

    inline bool Machine_grid_view::try_get_neighbour_tile(
        const Hex_coord xy,
        const HexFacingName facing,
        Hex_coord &c) const
    {
        return grid_->try_get_neighbour_tile(xy, facing, c);
    }

    inline void Machine_grid_view::get_sorted_circle_contents(
        const Hex_coord xy,
        const std::uint16_t r,
        std::vector<Hex_grid::Hex_tile> &v,
        HexContentFlags flags) const
    {
        grid_->get_sorted_circle_contents(xy, r, v, flags);
    }

    inline const Hex_grid &Machine_grid_view::grid() const
    {
        return grid_->grid();
    }

    inline bool Machine_grid_view::has_grid() const
    {
        return grid_ != nullptr;
    }
}
