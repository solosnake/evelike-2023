#include "solosnake/iscreen_factory.hpp"
#include "solosnake/waitcursor.hpp"
#include "solosnake/iscreen.hpp"

namespace solosnake
{
    iscreen_factory::~iscreen_factory()
    {
    }

    std::unique_ptr<iscreen> iscreen_factory::create_screen( const nextscreen& scr ) const
    {
        auto waiting = waitcursor::make_sentinel();
        return this->do_create_screen( scr );
    }
}
