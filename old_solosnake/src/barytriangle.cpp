#include "solosnake/barytriangle.hpp"

namespace solosnake
{
    barytriangle2d::barytriangle2d( const point2_t v1, const point2_t v2, const point2_t v3 )
        : x3_( v3[0] )
        , y3_( v3[1] )
        , y2_y3_( v2[1] - v3[1] )
        , x1_x3_( v1[0] - v3[0] )
        , x3_x2_( v3[0] - v2[0] )
        , y3_y1_( v3[1] - v1[1] )
        , divider_( ( ( v2[1] - v3[1] ) * ( v1[0] - v3[0] ) ) + ( ( v3[0] - v2[0] ) * ( v1[1] - v3[1] ) ) )
    {
    }

    barytriangle2d::barytriangle2d( float x1, float y1, float x2, float y2, float x3, float y3 )
        : x3_( x3 )
        , y3_( y3 )
        , y2_y3_( y2 - y3 )
        , x1_x3_( x1 - x3 )
        , x3_x2_( x3 - x2 )
        , y3_y1_( y3 - y1 )
        , divider_( ( ( y2 - y3 ) * ( x1 - x3 ) ) + ( ( x3 - x2 ) * ( y1 - y3 ) ) )
    {
    }

    bool barytriangle2d::point_is_outside( float x, float y ) const
    {
        point3_t barycoord;
        get_barycentric_coords( x, y, barycoord );

        return
            barycoord[0] < 0.0f ||
            barycoord[0] > 1.0f ||
            barycoord[1] < 0.0f ||
            barycoord[1] > 1.0f ||
            barycoord[2] < 0.0f ||
            barycoord[2] > 1.0f;
    }
}
