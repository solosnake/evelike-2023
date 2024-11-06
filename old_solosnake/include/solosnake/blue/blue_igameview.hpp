#ifndef blue_igameview_hpp
#define blue_igameview_hpp

#include "solosnake/blue/blue_iboardview.hpp"

namespace blue
{
    class Machine;
    struct Hex_coord;

    //! Model/view interface of game exposed to widgets. This is the set of functions that
    //! pages with a game in them can use to interact with the game.
    class igameview : public iboardview
    {
    public:

        //! Returns a const pointer to the machine at the coordinate @a xy, or nullptr
        //! if there is no machine on that tile.
        virtual const Machine* get_machine_at( Hex_coord ) const = 0;

        //! Called to tell the game to highlight this tile. This is perhaps a bad member
        //! function - too specific, and its at the wrong level. Shouldn't the system detect
        //! instructions sent itself?
        //! TODO: Review instructions_sent_highlight.
        virtual void instructions_sent_highlight( Hex_coord )    = 0;
    };
}

#endif
