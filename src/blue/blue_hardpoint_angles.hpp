#ifndef blue_hardpoint_angles_hpp
#define blue_hardpoint_angles_hpp

#include "solosnake/blue/blue_fixed_angle.hpp"

namespace blue
{
    struct Hex_coord;

    /// Returns a number between 0 and BLUE_TWO_PI which is the angle
    /// from @a location to @a targetLocation, relative to @a facingAngle.
    /// If location and targetLocation are the same location, returns 0.
    Fixed_angle relative_angle_to_target( Fixed_angle facingAngle,
                                          Hex_coord location,
                                          Hex_coord targetLocation );

    /// Due to the nature of a circle, the min angle can be a higher number
    /// than the max e.g. if the range was between 300 degrees and 10
    /// degrees.
    bool in_angle_range( Fixed_angle angle,
                         Fixed_angle minAngle,
                         Fixed_angle maxAngle );
}

#endif
