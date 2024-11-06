#ifndef blue_deck_hpp
#define blue_deck_hpp

#include <memory>
#include <string>
#include "solosnake/blue/blue_designedmachine.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    //! The "deck" is the choice of things the player takes into the game with him. It is
    //! intended to be like a 'deck' of cards in Magic or other tactical games. The player
    //! chooses a subset of objects, usually restricted by some point limit, and pits this
    //! choice against his opponent.
    class deck
    {
    public:

        explicit deck(
            const std::shared_ptr<player>& owner,
            const std::vector<designedmachine>& machines,
            const Tradables& resources);

        //! Returns the owner of this deck. This will throw if the owning
        //! player has gone out of scope.
        std::shared_ptr<player> deck_owner() const;

    private:

        std::weak_ptr<player>           owner_;         //!< The player who owns this deck.
        Tradables                       resources_;     //!< Limits the number of machines created.
        std::vector<designedmachine>    machines_;
    };
}

#endif
