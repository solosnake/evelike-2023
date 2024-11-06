#ifndef blue_controllers_gamestate_hpp
#define blue_controllers_gamestate_hpp

#include <vector>
#include <memory>
#include "solosnake/byte.hpp"
#include "solosnake/bytebuffer.hpp"
#include "solosnake/blue/blue_boardobserver.hpp"

namespace blue
{
    class gameaction;
    class Board_state;

    //! The state of the board including selected elements etc. There is the
    //! inner board state, which is the game on its own, and this is the game
    //! including the players concepts.
    //! This class serves to own and insulate/encapsulate the Board_state
    //! from all possible external changes.
    //!
    //! The state can only be changed via actions which are passed in when
    //! advancing the frame.
    //! The gamestate can be shared, the Board_state object is never.
    class gamestate
    {
    public:

        explicit gamestate( std::unique_ptr<Board_state> );

        ~gamestate();

        const Board_state* boardgamestate() const  { return state_.get(); }

        void add_observer( const std::shared_ptr<boardobserver>& );

        void remove_observer( const std::shared_ptr<boardobserver>& );

        //! Actions passed in here change the game state. This is the only way
        //! to change the state.
        void advance_one_frame( const std::vector<gameaction>& );

    private:

        gamestate( const gamestate& );

        gamestate& operator=( const gamestate& );

    private:

        std::unique_ptr<Board_state> state_;
    };

}

#endif
