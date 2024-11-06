#include "solosnake/blue/blue_thruster_attachpoint.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Thruster_attachpoint::Thruster_attachpoint() : Thruster_attachpoint(81)
    {
    }

    Thruster_attachpoint::Thruster_attachpoint( unsigned short n ) : AttachPoint( n )
    {
        if( !is_thruster_attachpoint() )
        {
            ss_throw( "Attach location is not valid Thruster attach point." );
        }
    }

    bool Thruster_attachpoint::operator < (const Thruster_attachpoint& rhs ) const noexcept
    {
        return this->location_number() < rhs.location_number();
    }

    bool Thruster_attachpoint::operator == (const Thruster_attachpoint& rhs ) const noexcept
    {
        return AttachPoint::operator==(rhs);
    }

    bool Thruster_attachpoint::operator != (const Thruster_attachpoint& rhs ) const noexcept
    {
        return AttachPoint::operator!=(rhs);
    }
}
