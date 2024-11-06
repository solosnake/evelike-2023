#include <cassert>
#include "solosnake/blue/blue_board_state_grid_view.hpp"
#include "solosnake/blue/blue_machine_grid_view.hpp"
#include "solosnake/blue/blue_movement_grid_view.hpp"
#include "solosnake/blue/blue_machine.hpp"

namespace blue
{
    Board_state_grid_view::Board_state_grid_view( const Game_hex_grid& g )
        : grid_( g )
    {
    }

    Board_state_grid_view::Board_state_grid_view( Game_hex_grid&& g )
        : grid_( std::move( g ) )
    {
    }

    void Board_state_grid_view::add_machine_to_board( std::unique_ptr<Machine> m,
                                                      Hex_coord xy,
                                                      HexFacingName facing )
    {
        m->add_to_board( xy, facing, Machine_grid_view( grid_ ) );
        grid_.place_bot_on_tile( xy );
        machines_.push_back( std::move( m ) );
        sort_machines_by_coord();
    }

    bool Board_state_grid_view::bot_is_on_board( const Machine* m ) const
    {
        return is_bot_on_tile( grid_.contents( m->board_xy() ) )
               && m == machine_at( m->board_xy() );
    }

    const std::vector<Machine*>& Board_state_grid_view::find_living_machines()
    {
        living_machines_.clear();

        const size_t n = machines_.size();

        for( size_t i = 0u; i < n; ++i )
        {
            Machine* m = machines_[i].get();

            if( m->is_alive() )
            {
                assert( bot_is_on_board( m ) );
                living_machines_.push_back( m );
            }
        }

        return living_machines_;
    }

    void Board_state_grid_view::cull_dead_machines()
    {
        // We may have new living machines if some were
        // built during the turn.

        for( auto it = machines_.begin(); it != machines_.end(); )
        {
            Machine* m = it->get();

            if( m->is_dead() )
            {
                // Machine was alive at start of frame but is dead now.
                m->removed_from_board();

                grid_.remove_bot_from_tile( m->board_xy() );

                if( m->is_being_watched() )
                {
                    // Move dead but watched machines to separate array.
                    dead_but_watched_machines_.push_back( std::move( *it ) );
                }

                it = machines_.erase( it );
            }
            else
            {
                ++it;
            }
        }

        // Check if any watched machines are no longer being watched.
        for( auto it = dead_but_watched_machines_.begin();
                it != dead_but_watched_machines_.end(); )
        {
            if( it->get()->is_not_being_watched() )
            {
                it = dead_but_watched_machines_.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    Machine* Board_state_grid_view::machine_at( Hex_coord xy ) const
    {
        auto i = std::lower_bound(
                     machines_.begin(),
                     machines_.end(),
                     xy,
                     [ = ]( const std::unique_ptr<Machine>& m, Hex_coord c )
        { return m->board_xy() < c; } );

        return ( ( i == machines_.end() ) || ( i->get()->board_xy() != xy ) ) ? nullptr : i->get();
    }

    void Board_state_grid_view::sort_machines_by_coord()
    {
        std::sort(
            machines_.begin(),
            machines_.end(),
            [ = ]( const std::unique_ptr<Machine>& lhs, const std::unique_ptr<Machine>& rhs )
        { return lhs->board_xy() < rhs->board_xy(); } );
    }

    void Board_state_grid_view::remove_bot_from_board( Machine* m )
    {
        assert( m );
        grid_.remove_bot_from_tile( m->board_xy() );
        m->removed_from_board();
    }

    void Board_state_grid_view::move_bot_from_tile_to_tile( const Hex_coord from, const Hex_coord to )
    {
        grid_.move_bot_from_tile_to_tile( from, to );
        sort_machines_by_coord();
    }

    Movement_grid_view Board_state_grid_view::as_movement_grid_view() noexcept
    {
        return Movement_grid_view( *this );
    }

    bool Board_state_grid_view::operator == (const Board_state_grid_view& rhs) const noexcept
    {
        ss_throw("Not implemented: Board_state_grid_view::operator ==");
        //
        //return  grid_ == rhs.grid_
        //    &&  living_machines_ == rhs.living_machines_
        //    &&  machines_ == rhs.machines_
        //    &&  dead_but_watched_machines_ == rhs.dead_but_watched_machines_
        //    ;
    }

    bool Board_state_grid_view::operator != (const Board_state_grid_view& rhs) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
