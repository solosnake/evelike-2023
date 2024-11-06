#include "solosnake/blue/blue_board_state.hpp"

namespace blue
{
    inline size_t Board_state::asteroids_count() const
    {
        return asteroids_.size();
    }

    inline size_t Board_state::machines_count() const
    {
        return grid_.machines_count();
    }

    inline size_t Board_state::suns_count() const
    {
        return suns_.size();
    }

    inline const Asteroid& Board_state::asteroid_at( size_t n ) const
    {
        return asteroids_[n];
    }

    inline const Sun& Board_state::sun_at( size_t n ) const
    {
        return suns_[n];
    }

    inline const Machine& Board_state::machine_at( size_t n ) const
    {
        assert( n < grid_.machines_count() );
        return (*grid_[n]);
    }

    inline const Game_hex_grid& Board_state::hex_board() const
    {
        return grid_.hex_board();
    }

    inline const Hex_grid& Board_state::grid() const
    {
        return grid_.hex_board().grid();
    }

    inline const std::vector<team>& Board_state::teams() const
    {
        return teams_;
    }

    inline unsigned int Board_state::grid_width() const
    {
        return grid_.grid_width();
    }

    inline unsigned int Board_state::grid_height() const
    {
        return grid_.grid_height();
    }
}
