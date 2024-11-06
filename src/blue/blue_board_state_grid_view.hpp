#ifndef blue_board_state_grid_view_hpp
#define blue_board_state_grid_view_hpp

#include <cstdint>
#include <memory>
#include <vector>
#include "solosnake/blue/blue_hexfacingname.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"

namespace blue
{
    class Machine;
    class Movement_grid_view;

    /// Enforces the constraints between the grid and the machines on it.
    class Board_state_grid_view
    {
    public:

        explicit Board_state_grid_view( const Game_hex_grid& );

        explicit Board_state_grid_view( Game_hex_grid&& );

        const std::vector<Machine*>& find_living_machines();

        void add_machine_to_board( std::unique_ptr<Machine>, Hex_coord, HexFacingName );

        void cull_dead_machines();

        Machine* machine_at( Hex_coord xy ) const;

        /// Returns the number of machines in the machines array. This is not any guarantee about
        /// the state of the machines (alive or dead).
        size_t machines_count() const;

        Machine* operator[]( size_t i );

        const Machine* operator[]( size_t i ) const;

        HexContentFlags contents( Hex_coord ) const;

        void contents_under_line(
            const Hex_coord A,
            const Hex_coord B,
            const std::uint16_t maxSteps,
            std::vector<Hex_coord>& v ) const;

        void get_sorted_circle_contents(
            const Hex_coord,
            const std::uint16_t r,
            std::vector<Hex_grid::Hex_tile>& tiles,
            HexContentFlags flags = 0u ) const;

        bool try_get_neighbour_tile(
            const Hex_coord xy,
            const HexFacingName facing,
            Hex_coord& c ) const;

        unsigned int grid_width() const noexcept;

        unsigned int grid_height() const noexcept;

        bool is_tile_reserved( const Hex_coord ) const noexcept;

        bool is_tile_empty_and_unreserved( const Hex_coord ) const noexcept;

        bool is_on_board( const Hex_coord ) const noexcept;

        bool bot_is_on_board( const Machine* ) const;

        void unreserve_empty_tile( const Hex_coord );

        void reserve_empty_tile( const Hex_coord );

        void place_asteroid_on_tile( const Hex_coord );

        void remove_asteroid_from_tile( const Hex_coord );

        void place_sun_on_tile( SunStrength strength, const Hex_coord );

        void move_bot_from_tile_to_tile( const Hex_coord from, const Hex_coord to );

        Movement_grid_view as_movement_grid_view() noexcept;

        const Game_hex_grid& hex_board() const noexcept;

        const Hex_grid& grid() const noexcept;

        bool operator == (const Board_state_grid_view& ) const noexcept;
        bool operator != (const Board_state_grid_view& ) const noexcept;

    private:

        void remove_bot_from_board( Machine* );

        void sort_machines_by_coord();

        Game_hex_grid                               grid_;
        std::vector<Machine*>                       living_machines_;
        std::vector<std::unique_ptr<Machine>>       machines_;
        std::vector<std::unique_ptr<Machine>>       dead_but_watched_machines_;
    };
}

#include "solosnake/blue/blue_board_state_grid_view.inl"
#endif
