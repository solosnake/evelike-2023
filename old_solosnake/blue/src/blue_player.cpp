#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_gameaction.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_instructions.hpp"

namespace blue
{
    player::~player()
    {
    }

    void player::send_instructions_to_board( Hex_coord xy,
                                             const std::shared_ptr<const instructions>& code )
    {
        playeractions_.emplace_back( gameaction::make_send_instructions_to_board( xy, *code ) );
    }

    std::vector<gameaction> player::take_actions()
    {
        std::vector<gameaction> actions;
        actions.swap( playeractions_ );
        return actions;
    }
}
