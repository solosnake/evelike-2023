#include "solosnake/power_of_2.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    power_of_2::power_of_2( unsigned int i ) : value_( 1u )
    {
        // sizeof operator returns size in bytes.
        if( i > ( 8 * sizeof( unsigned int ) ) )
        {
            ss_throw( "Cannot represent this power of 2." );
        }

        value_ <<= i;
    }
}
