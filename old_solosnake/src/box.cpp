#include "solosnake/box.hpp"

namespace solosnake
{
    box::box() : corner_( 0.0f, 0.0f, 0.0f ), size_( 0.0f, 0.0f, 0.0f )
    {
    }

    box::box( const point3d& corner, const dimension3df& size ) : corner_( corner ), size_( size )
    {
    }

    bool box::contains( const point3d& p ) const
    {
        return ( p.xyz[0] >= corner_.xyz[0] ) && ( p.xyz[0] <= ( corner_.xyz[0] + size_.x() ) )
               && ( p.xyz[1] >= corner_.xyz[1] ) && ( p.xyz[1] <= ( corner_.xyz[1] + size_.y() ) )
               && ( p.xyz[2] >= corner_.xyz[2] ) && ( p.xyz[2] <= ( corner_.xyz[2] + size_.z() ) );
    }

    point3d box::nearest_point_to( const point3d& p ) const
    {
        point3d nearest = p;

        const float whd[3] = { size_.width(), size_.height(), size_.depth() };

        for( size_t i = 0; i < 3; ++i )
        {
            if( nearest.xyz[i] < corner_.xyz[i] )
            {
                nearest.xyz[i] = corner_.xyz[i];
            }
            else if( nearest.xyz[i] > ( corner_.xyz[i] + whd[i] ) )
            {
                nearest.xyz[i] = ( corner_.xyz[i] + whd[i] );
            }
        }

        return nearest;
    }

    box box::shrunk_by( float amount ) const
    {
        float newsize[3];
        float newcorner[3];

        const float halfamount = amount / 2.0f;
        const float whd[3] = { size_.width(), size_.height(), size_.depth() };
        const point3d centrePoint = centre();

        for( size_t i = 0; i < 3; ++i )
        {
            if( whd[i] >= amount )
            {
                newsize[i] = whd[i] - halfamount;
                newcorner[i] = corner_.xyz[i] + halfamount;
            }
            else
            {
                newsize[i] = 0.0f;
                newcorner[i] = centrePoint.xyz[i];
            }
        }

        return box( point3d( newcorner ), dimension3df( newsize[0], newsize[1], newsize[2] ) );
    }
}
