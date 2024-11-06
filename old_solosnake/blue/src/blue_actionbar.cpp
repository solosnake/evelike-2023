#include "solosnake/blue/blue_actionbar.hpp"
#include <cassert>

using namespace std;
using namespace solosnake;

namespace blue
{
    actionbar::actionbar(
        const std::shared_ptr<deck>& dck )
        : players_deck_( dck )
    {
        assert( players_deck_ );
    }


    unsigned int actionbar::button_count() const
    {
        return 10u;
    }
}
