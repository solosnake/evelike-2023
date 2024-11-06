#ifndef blue_selecteddeck_hpp
#define blue_selecteddeck_hpp

#include <memory>
#include <string>

// Functions related to loading and storing the players selected deck.

namespace blue
{    
    class datapaths;
    class deck;
    class player;
    class user_settings;

    //! Stores the selected decks name.
    void store_selected_decks_name( const std::string& deckname, user_settings& );

    //! Returns the currently preferred deck, the player's choice of deck, stored
    //! in the user_settings. 
    std::string get_selected_decks_name( const user_settings& );

    //! Loads the deck named @a deckname from file and returns it.
    std::shared_ptr<deck> load_selected_deck( const std::string& deckname,
                                              const std::shared_ptr<player>& deckowner,
                                              const std::shared_ptr<datapaths>& );
}

#endif
