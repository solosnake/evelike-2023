#include "solosnake/blue/blue_hardware_factor.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Hardware_factor::Hardware_factor( const float f ) : value_( f )
    {
        if( value_ < 0.0f )
        {
            ss_throw( "Invalid factor, factor is < 0.0." );
        }
    }
}
