#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_sun.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Sun::Sun( const SunStrength s, const SunType t, const Hex_coord xy )
     : xy_( xy )
     , strength_( static_cast<std::uint16_t>( s ) )
     , type_( t )
    {
    }

    float Sun::radius() const
    {
        // TODO CORRECT SUN STRENGTH TO RADIUS
        ss_wrn( "INCORRECT SUN STRENGTH" );
        return 1.0f;
    }

}
