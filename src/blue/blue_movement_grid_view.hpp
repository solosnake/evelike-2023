#ifndef blue_movement_grid_view_hpp
#define blue_movement_grid_view_hpp

#include <cstdint>

namespace blue
{
    class Board_state_grid_view;
    struct Hex_coord;
    enum HexFacingName : std::int16_t;

    /// Class to restrict the permitted changes to the grid possible
    /// by Machine's during their movement phase.
    class Movement_grid_view
    {
    public:

        explicit Movement_grid_view( Board_state_grid_view& );

        bool is_on_board( const Hex_coord ) const;

        bool is_tile_reserved( const Hex_coord ) const;

        bool is_tile_empty_and_unreserved( const Hex_coord ) const;

        bool try_get_neighbour_tile( const Hex_coord xy, const HexFacingName facing, Hex_coord& c ) const;

        void move_bot_from_tile_to_tile( const Hex_coord from, const Hex_coord to );

        void reserve_empty_tile( const Hex_coord );

    private:

        Board_state_grid_view* grid_;
    };
}

#endif
