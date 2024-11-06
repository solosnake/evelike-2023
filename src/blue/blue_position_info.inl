#include "solosnake/blue/blue_position_info.hpp"

namespace blue
{
    inline void Position_info::board_offset_after_n_frames( const float frames, float* unaliased xy ) const
    {
        const auto a = blue::fixed_angle_to_radians( angle );
        xy[0] = ( a.cos() * ( distance + ( dXYdt * frames ) ) ) / BLUE_FLOAT32_TILE_WIDTH;
        xy[1] = ( a.sin() * ( distance + ( dXYdt * frames ) ) ) / BLUE_FLOAT32_TILE_WIDTH;
    }

    inline Radians Position_info::angle_as_radians() const
    {
        return blue::fixed_angle_to_radians( angle );
    }

    inline bool operator == ( const Position_info& lhs, const Position_info& rhs )
    {
        return lhs.distance    == rhs.distance
               && lhs.dXYdt    == rhs.dXYdt
               && lhs.angle    == rhs.angle
               && lhs.gridXY   == rhs.gridXY
               ;
    }

    inline bool operator != ( const Position_info& lhs, const Position_info& rhs )
    {
        return lhs.distance    != rhs.distance
               || lhs.dXYdt    != rhs.dXYdt
               || lhs.angle    != rhs.angle
               || lhs.gridXY   != rhs.gridXY
               ;
    }
}