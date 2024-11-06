#ifndef blue_machinegridview_hpp
#define blue_machinegridview_hpp

#include "solosnake/blue/blue_game_hex_grid.hpp"

namespace blue
{
    /// Not intended as a pure virtual base class, this is the interface
    /// which restricts the operations a Machine can perform on a board.
    /// This is the Machine object's view of, and access level to, the grid.
    class Machine_grid_view
    {
    public:

        Machine_grid_view() = default;

        explicit Machine_grid_view( Game_hex_grid& g );

        void            reserve_empty_tile( const Hex_coord );

        HexContentFlags contents( Hex_coord ) const;

        size_t          get_step_distance_between( const Hex_coord, const Hex_coord ) const;

        bool            is_on_board( const Hex_coord ) const;

        bool            is_tile_empty_and_unreserved( const Hex_coord ) const;

        bool            try_get_neighbour_tile( const Hex_coord, const HexFacingName, Hex_coord& ) const;

        SunStrength     tile_sun_strength( Hex_coord ) const;

        void            get_sorted_circle_contents( const Hex_coord,
                                                    const std::uint16_t,
                                                    std::vector<Hex_grid::Hex_tile>&,
                                                    HexContentFlags ) const;

        const Hex_grid&  grid() const;

        bool            has_grid() const;

    private:

        Game_hex_grid* grid_ = nullptr;
    };
}

#include "solosnake/blue/blue_machine_grid_view.inl"
#endif
