#include "solosnake/blue/blue_deck.hpp"
#include "solosnake/blue/blue_designedmachine.hpp"
#include "solosnake/blue/blue_selecteddeck.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include <memory>
#include <vector>

namespace blue
{
    void store_selected_decks_name( const std::string& deckname, user_settings& settings )
    {
        settings.set( BLUE_GAME_SELECTED_DECK, deckname, BLUE_GAME_SELECTED_DECK_COMMENT ); 
    }

    std::string get_selected_decks_name( const user_settings& settings )
    {
        return settings.value( BLUE_GAME_SELECTED_DECK ).as_string();
    }

    //! Loads the deck named @a deckname from file and returns it.
    std::shared_ptr<deck> load_selected_deck( const std::string& deckname,
                                              const std::shared_ptr<player>& deckowner,
                                              const std::shared_ptr<datapaths>& )
    {
        // DAIRE TODO
        std::vector<designedmachine> machines;
        tradables resources;
        return std::make_shared<deck>( deckowner, machines, resources );
    }
}
