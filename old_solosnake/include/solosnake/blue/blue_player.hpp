#ifndef blue_player_hpp
#define blue_player_hpp

#include <memory>
#include <vector>
#include "solosnake/blue/blue_gameaction.hpp"
#include "solosnake/blue/blue_igameactions.hpp"

namespace blue
{
    struct Hex_coord;
    class Instructions;

    //! Represents a player and what he can do to the board.
    //! The things a player can DO in the game to the BOARD and its CONTENTS.
    //! A player can be real or AI.
    class player : public igameactions
    {
    public:

        virtual ~player();

        virtual void send_instructions_to_board( Hex_coord, const std::shared_ptr<const Instructions>& );

        //! Clear the player's accumulated actions and returns them.
        std::vector<gameaction> take_actions();

    private:

        std::vector<gameaction> playeractions_;
    };

}

#endif
