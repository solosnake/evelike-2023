#ifndef blue_controllers_gamestate_hpp
#define blue_controllers_gamestate_hpp

#include <memory>
#include "solosnake/byte.hpp"
#include "solosnake/blue/blue_gamestatechange_accumulator.hpp"

namespace blue
{
    class Board_state;

    /**
        * The controllers have a slightly different 'model' to track then simply
        * the raw board-game state. They also track selection sets and visual
        * components that are not related to game play.
        */
    class controllers_gamestate
    {
    public:
        controllers_gamestate( std::shared_ptr<Board_state> );

        ~controllers_gamestate();

        const Board_state* boardgamestate() const
        {
            return pstate_;
        }

        void process_commands( const solosnake::network::byte* );

    private:
        controllers_gamestate( const controllers_gamestate& );
        controllers_gamestate& operator=( const controllers_gamestate& );

    private:
        gamestatechange_accumulator_ptr change_event_accumulator_;
        std::shared_ptr<Board_state> state_;
        Board_state* pstate_;
    };

    typedef std::shared_ptr<controllers_gamestate> controllers_gamestate_ptr;
}

#endif
