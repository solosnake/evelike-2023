#include "solosnake/blue/blue_hardpoint_angles.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include <cassert>
#include <cmath>

namespace blue
{
    Fixed_angle relative_angle_to_target( Fixed_angle facingAngle,
                                          Hex_coord location,
                                          Hex_coord targetLocation )
    {
        assert( facingAngle >= 0 );
        assert( facingAngle <= BLUE_INT32_TWO_PI );

        if( location == targetLocation )
        {
            return Fixed_angle( 0 );
        }

        float location_XY[2];
        float target_XY[2];
        Hex_grid::calculate_xy( location, location_XY );
        Hex_grid::calculate_xy( targetLocation, target_XY );
        target_XY[0] -= location_XY[0];
        target_XY[1] -= location_XY[1];

        // atan2 returns the principal arc tangent of y/x, in the interval [-pi,+pi] radians.
        auto rads = std::atan2( target_XY[1], target_XY[0] );

        // Move the angle into the positive without changing the actual heading.
        auto posRads = rads < 0 ? rads + BLUE_TWO_PI : rads;

        // Convert to 'fixed' system.
        Fixed_angle angle = static_cast<Fixed_angle>( BLUE_FLOAT32_FIXED_TWO_PI * (posRads / BLUE_TWO_PI) );

        assert( abs( angle ) <= BLUE_INT32_TWO_PI );
        assert( angle >= 0 && angle < BLUE_INT32_TWO_PI );

        // Get relative angle. This could be negative.
        Fixed_angle relativeAngle = angle - facingAngle;
        assert( abs(relativeAngle) <= BLUE_INT32_TWO_PI );

        // Clamp to between 0 and BLUE_TWO_PI.
        relativeAngle = ( relativeAngle + BLUE_INT32_TWO_PI ) % BLUE_INT32_TWO_PI;

        assert( relativeAngle >= 0 );
        assert( relativeAngle <= BLUE_INT32_TWO_PI );

        return relativeAngle;
    }

    bool in_angle_range( Fixed_angle angle, Fixed_angle minAngle, Fixed_angle maxAngle )
    {
        // Rotate all so that min angle lies at zero.
        const Fixed_angle r = BLUE_INT32_TWO_PI - minAngle;
        angle = ( angle + r ) % BLUE_INT32_TWO_PI;
        maxAngle = ( maxAngle + r ) % BLUE_INT32_TWO_PI;
        return angle >= 0 && angle <= maxAngle;
    }
}
