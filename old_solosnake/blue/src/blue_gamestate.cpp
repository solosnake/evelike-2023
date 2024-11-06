#include <cassert>
#include "solosnake/blue/blue_gamestate.hpp"
#include "solosnake/blue/blue_board_state.hpp"

using namespace solosnake;

namespace blue
{
    gamestate::gamestate( std::unique_ptr<boardstate> b ) : state_( std::move( b ) )
    {
    }

    gamestate::~gamestate()
    {
        ss_log( "Game over man!" );
    }

    void gamestate::advance_one_frame( const std::vector<gameaction>& actions )
    {
        state_->advance_one_frame( actions );
    }

    void gamestate::add_observer( const std::shared_ptr<boardobserver>& p )
    {
        state_->add_observer( p );
    }

    void gamestate::remove_observer( const std::shared_ptr<boardobserver>& p )
    {
        state_->remove_observer( p );
    }
}

// "$(ProjectFileName).lnt"