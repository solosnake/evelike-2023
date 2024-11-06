#include <cassert>
#include <cstdlib>
#include <iostream>
#include "solosnake/blue/blue_half_line_3d.hpp"
#include "solosnake/blue/blue_point3d.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    bool find_half_line_y_plane_intersection( const Half_line_3d& line,
                                              const float y,
                                              float* unaliased intersectionXYZ )
    {
        // Move to be relative to Y=0 plane
        const float head_y = line.head().xyz[1] - y;
        const float tail_y = line.tail().xyz[1] - y;

        const bool planeParallel = ( head_y == tail_y );
        const bool pointingUpAndAway = head_y > 0.0f && tail_y > 0.0f && tail_y > head_y;
        const bool pointingDownAndAway = head_y < 0.0f && tail_y < 0.0f && tail_y < head_y;

        if( planeParallel || pointingUpAndAway || pointingDownAndAway )
        {
            return false;
        }

        const Point3d L0( line.head().xyz[0], head_y, line.head().xyz[2] );
        const Point3d dir( line.direction() );
        const float D = ( -head_y ) / ( dir.xyz[1] );

        intersectionXYZ[0] = L0.xyz[0] + ( D * dir.xyz[0] );
        intersectionXYZ[1] = L0.xyz[1] + ( D * dir.xyz[1] ) + y;
        intersectionXYZ[2] = L0.xyz[2] + ( D * dir.xyz[2] );

        assert( std::abs( intersectionXYZ[1] - y ) < 0.001f );

        return true;
    }

    void Half_line_3d::normalize()
    {
        dir_ -= start_;
        if( dir_.normalize() < 0.0001f )
        {
            ss_throw("Half-line is too short to normalize.");
        }
    }
}

