#include "solosnake/blue/blue_deck.hpp"

namespace blue
{
    deck::deck(
        const std::shared_ptr<player>& owner,
        const std::vector<designedmachine>& machines,
        const tradables& resources )
        : owner_( owner )
        , resources_( resources )
        , machines_( machines )
    {
    }

    std::shared_ptr<player> deck::deck_owner() const
    {
        return std::shared_ptr<player>( owner_ );
    }
}
