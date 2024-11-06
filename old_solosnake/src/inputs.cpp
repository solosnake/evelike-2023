#include "solosnake/inputs.hpp"

namespace solosnake
{
    static_assert( sizeof( input_event ) == 8,
                   "input_event is not packed to 64 bits." );

    inputs::inputs() : is_shutdown_( false )
    {
        events_.reserve( 64 );
    }

    void inputs::clear_events()
    {
        events_.clear();
    }

    bool inputs::is_shutdown() const
    {
        return is_shutdown_;
    }

    void inputs::shutdown()
    {
        is_shutdown_ = true;
    }
}
