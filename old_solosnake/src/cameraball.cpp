#include "solosnake/cameraball.hpp"
#include "solosnake/matrix3d.hpp"

namespace solosnake
{
    cameraball::cameraball()
        : location_( 0.0f, 0.0f, 0.0f )
        , forwards_( 0.0f, 0.0f, -1.0f ) // Looking along negative Z axis.
        , up_( 0.0f, 1.0f, 0.0f )
        , radius_( 1.0f )
    {
        move_to( boundingBox_.centre() );
    }

    cameraball::cameraball( float radius, const point3d& startingPosition, const box& boundingBox )
        : location_( boundingBox.centre() )
        , forwards_( 0.0f, 0.0f, -1.0f ) // Looking along negative Z axis.
        , up_( 0.0f, 1.0f, 0.0f )
        , radius_( radius )
        , boundingBox_( boundingBox.shrunk_by( 2.0f * radius ) )
    {
        assert( radius > 0.0f );
        move_to( startingPosition );
    }

    void cameraball::lookat( const point3d& p )
    {
        // If point is too close (handles case of camera looking at itself)
        // move camera backwards.
        if( distance_squared3( p.xyz, location_.xyz ) < ( radius_ * radius_ ) )
        {
            // Moves backwards but within box.
            move_forwards( -radius_ );
        }

        // Calculate what the forwards might be and use it if it is not too
        // near to the camera location.
        point3d newForwards;
        sub3( p.xyz, location_.xyz, newForwards.xyz );
        const float distance = normalise3( newForwards.xyz );

        // Use new lookat/forwards if it outside the radius.
        if( distance >= radius_ )
        {
            forwards_ = newForwards;
        }
    }

    void cameraball::move_forwards( float nUnits )
    {
        float newlocation[3];
        madd3( forwards_.xyz, nUnits, location_.xyz, newlocation );
        move_to( point3d( newlocation ) );
    }

    void cameraball::move_to( const point3d& p )
    {
        location_ = boundingBox_.nearest_point_to( p );
    }

    void cameraball::set_into_view_matrix( float* unaliased result ) const
    {
        matrix4x4_t dst4x4;
        float side[3];
        float up[3];

        // Side = forward x up
        cross3( forwards_.xyz, up_.xyz, side );
        normalise3( side );

        // Recompute up as: up = side x forward
        cross3( side, forwards_.xyz, up );

        dst4x4[0] = side[0];
        dst4x4[1] = up[0];
        dst4x4[2] = -forwards_.xyz[0];
        dst4x4[3] = 0.0f;

        dst4x4[4] = side[1];
        dst4x4[5] = up[1];
        dst4x4[6] = -forwards_.xyz[1];
        dst4x4[7] = 0.0f;

        dst4x4[8] = side[2];
        dst4x4[9] = up[2];
        dst4x4[10] = -forwards_.xyz[2];
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0;

        matrix4x4_t trans;
        load_translation_4x4( -location_.xyz[0], -location_.xyz[1], -location_.xyz[2], trans );
        mul_4x4( trans, dst4x4, result );
    }
}
