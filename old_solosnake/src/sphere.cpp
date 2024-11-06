#include "solosnake/sphere.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    sphere::sphere() : centre_( 0.0f, 0.0f, 0.0f ), radius_( 0.0f )
    {
    }

    sphere::sphere( const point3d& pos, float radius ) : centre_( pos ), radius_( radius )
    {
        if( radius < 0.0f )
        {
            ss_throw( "Radius of sphere is negative." );
        }
    }

    bool sphere::contains( const point3d& p ) const
    {
        return distance_squared3( p.xyz, centre_.xyz ) <= ( radius_ * radius_ );
    }

    point3d sphere::nearest_point_to( const point3d& p ) const
    {
        if( contains( p ) )
        {
            return p;
        }

        if( radius_ <= 0.0f )
        {
            // Special case.
            return centre_;
        }

        float result[3];
        float linedir[3];

        sub3( p.xyz, centre_.xyz, linedir );
        normalise3( linedir );
        mul3( linedir, radius_ );
        add3( linedir, centre_.xyz, result );

        return point3d( result );
    }
}
