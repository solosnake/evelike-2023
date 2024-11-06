#ifndef blue_actionbar_hpp
#define blue_actionbar_hpp

#include <memory>
#include "solosnake/blue/blue_deck.hpp"

namespace blue
{
    //! The actionbar is the main control that the user interacts with when playing the game.
    //! It can add the player's ships to the board, broadcast code snippets etc.
    class actionbar
    {
    public:

        actionbar(
            const std::shared_ptr<deck>& );

        unsigned int button_count() const;

    private:

        std::shared_ptr<deck>           players_deck_;
    };
}

#endif
