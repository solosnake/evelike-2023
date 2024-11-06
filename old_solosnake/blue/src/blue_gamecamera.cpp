#include "solosnake/blue/blue_gamecamera.hpp"
#include "solosnake/clamp.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/matrix3d.hpp"

using namespace solosnake;

#define SS_DEFAULT_BOARDCAMERA_ZOOMFACTOR (2.0f)

namespace blue
{
    gamecamera::gamecamera()
        : boundingSphere_( point3d( 0.0f, 0.0f, 0.0f ), 1.0f )
        , tilt_( ZERO_RADIANS )
        , mintilt_( ZERO_RADIANS )
        , maxtilt_( HALFPI_RADIANS )
        , rotation_( ZERO_RADIANS )
        , lookedat_( 0.0f, 1.0f, 0.0f )
        , armlength_( 1.0f )
        , radius_( 1.0f )
        , zoomfactor_( SS_DEFAULT_BOARDCAMERA_ZOOMFACTOR )
    {
    }

    gamecamera::gamecamera( const float boardY,
                            const float radius,
                            const float armlength,
                            const float lookingatX,
                            const float lookingatZ,
                            const radians& tilt,
                            const radians& mintilt,
                            const radians& maxtilt,
                            const radians& rotation,
                            const sphere& boundry,
                            const float zoomFactor )
        : boundingSphere_( boundry.shrunk_by( radius ) )
        , tilt_( clamp( tilt, mintilt, maxtilt ) )
        , mintilt_( mintilt )
        , maxtilt_( maxtilt )
        , rotation_( rotation )
        , lookedat_( lookingatX, boardY, lookingatZ )
        , armlength_( std::max( armlength, radius ) )
        , radius_( radius )
        , zoomfactor_( zoomFactor )
    {
        if( mintilt > maxtilt )
        {
            ss_throw( "mintilt > maxtilt: invalid tilt params." );
        }

        if( tilt < mintilt || tilt > maxtilt )
        {
            ss_throw( "tilt out of range." );
        }

        if( radius <= 0.0f )
        {
            ss_throw( "Invalid radius for gamecamera." );
        }

        if( radius_ > boundry.radius() )
        {
            ss_throw( "Camera radius exceeded bounding radius." );
        }

        location_ = calculate_location();

        if( false == boundingSphere_.contains( location_ ) )
        {
            ss_throw( "gamecamera start location is not inside bounding sphere." );
        }
    }

    point3d gamecamera::calculate_location() const
    {
        const float sintilt = tilt_.sin();
        const float costilt = tilt_.cos();
        const float sinrote = rotation_.sin();
        const float cosrote = rotation_.cos();
        const float armY = armlength_ * sintilt;
        const float lengthProj = armlength_ * costilt;

        const float x = lookedat_.xyz[0] + lengthProj * cosrote;
        const float y = lookedat_.xyz[1] + armY;
        const float z = lookedat_.xyz[2] + lengthProj * sinrote;

        return point3d( x, y, z );
    }

    void gamecamera::rotate( const radians r )
    {
        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );

        const radians oldRotation = rotation_;
        rotation_ = ( rotation_ + r ).clamped();
        point3d possibleLocation = calculate_location();

        if( boundingSphere_.contains( possibleLocation ) )
        {
            location_ = possibleLocation;
        }
        else
        {
            // No movement so restore previous rotation.
            rotation_ = oldRotation;
        }

        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );
    }

    //void gamecamera::tilt( radians r )
    //{
    //    assert( location_ == calculate_location() );
    //    assert( boundingSphere_.contains( location_ ) );

    //    const radians oldTilt = tilt_;
    //    tilt_ = ( tilt_ + r ).clamped();

    //    // Clamp to between zero and ninety.
    //    if( tilt_ < mintilt_ )
    //    {
    //        tilt_ = mintilt_;
    //    }
    //    else if( tilt_ > maxtilt_ )
    //    {
    //        tilt_ = maxtilt_;
    //    }

    //    point3d possibleLocation = calculate_location();

    //    if( boundingSphere_.contains( possibleLocation ) )
    //    {
    //        location_ = possibleLocation;
    //    }
    //    else
    //    {
    //        // No movement so restore previous tilt.
    //        tilt_ = oldTilt;
    //    }

    //    assert( tilt_ >= mintilt_ && tilt_ <= maxtilt_ );
    //    assert( location_ == calculate_location() );
    //    assert( boundingSphere_.contains( location_ ) );
    //}

    void gamecamera::translate( const float x, const float z )
    {
        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );

        const float oldLookedatX = lookedat_.xyz[0];
        const float oldLookedatZ = lookedat_.xyz[2];
        lookedat_.xyz[0] += x;
        lookedat_.xyz[2] += z;
        point3d possibleLocation = calculate_location();

        if( boundingSphere_.contains( possibleLocation ) )
        {
            location_ = possibleLocation;
        }
        else
        {
            // No movement so restore previous lookat.
            lookedat_.xyz[0] = oldLookedatX;
            lookedat_.xyz[2] = oldLookedatZ;
        }

        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );
    }

    void gamecamera::move_towards_lookat( const float amount )
    {
        const float amt = amount * zoomfactor_;

        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );

        const float oldArmLength = armlength_;

        // Subtract the amount: negative movement forwards means moving back
        // which means making the armlength longer.
        armlength_ = std::max( armlength_ - amt, radius_ );

        if( armlength_ != oldArmLength )
        {
            point3d possibleLocation = calculate_location();

            if( boundingSphere_.contains( possibleLocation ) )
            {
                location_ = possibleLocation;
            }
            else
            {
                // No movement so restore previous arm length.
                armlength_ = oldArmLength;
            }
        }

        assert( armlength_ <= bounding_radius() );
        assert( location_ == calculate_location() );
        assert( boundingSphere_.contains( location_ ) );
    }

    solosnake::point3d gamecamera::forwards() const
    {
        assert( location_ == calculate_location() );

        // A safe up value that should work in all angles.
        point3d up( rotation_.cos(), 1.414f, rotation_.sin() );

        float forwards[3];

        sub3( lookedat_.xyz, location_.xyz, forwards );
        normalise3( forwards );

        return solosnake::point3d( forwards );
    }

    solosnake::point3d gamecamera::upwards() const
    {
        assert( location_ == calculate_location() );

        // A safe up value that should work in all angles.
        point3d up( rotation_.cos(), 1.414f, rotation_.sin() );

        float side[3];
        float forwards[3];

        sub3( lookedat_.xyz, location_.xyz, forwards );
        normalise3( forwards );

        // Side = forward x up
        cross3( forwards, up.xyz, side );
        normalise3( side );

        // Recompute up as: up = side x forward
        cross3( side, forwards, up.xyz );

        return solosnake::point3d( up );
    }

    void gamecamera::get_orientation( float* unaliased up,
                                      float* unaliased side,
                                      float* unaliased ahead ) const
    {
        assert( location_ == calculate_location() );

        // A safe up value that should work in all angles.
        up[0] = rotation_.cos();
        up[1] = 1.414f;
        up[2] = rotation_.sin();

        sub3( lookedat_.xyz, location_.xyz, ahead );
        normalise3( ahead );

        // Side = forward x up
        cross3( ahead, up, side );
        normalise3( side );

        // Recompute up as: up = side x forward
        cross3( side, ahead, up );
    }

    void gamecamera::get_view_matrix( float* unaliased result ) const
    {
        assert( location_ == calculate_location() );

        float up[3];
        float side[3];
        float forwards[3];
        get_orientation( up, side, forwards );

        matrix4x4_t dst4x4;
        dst4x4[0] = side[0];
        dst4x4[1] = up[0];
        dst4x4[2] = -forwards[0];
        dst4x4[3] = 0.0f;

        dst4x4[4] = side[1];
        dst4x4[5] = up[1];
        dst4x4[6] = -forwards[1];
        dst4x4[7] = 0.0f;

        dst4x4[8] = side[2];
        dst4x4[9] = up[2];
        dst4x4[10] = -forwards[2];
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
