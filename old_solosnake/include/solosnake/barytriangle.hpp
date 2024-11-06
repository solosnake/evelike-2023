#ifndef solosnake_barytriangle_hpp
#define solosnake_barytriangle_hpp

#include "solosnake/point.hpp"

namespace solosnake
{
    //! Uses barycentric coordinates in 2D to determine the coordinates
    //! of a given point wrt to the specified triangle. This can
    //! be used to test if the point is inside the triangle.
    //! http://en.wikipedia.org/wiki/Barycentric_coordinates_%28mathematics%29
    //! If a point lies in the interior of the triangle, all of the Barycentric
    //! coordinates lie in the open interval (0,1). If a point lies on an edge
    //! of the triangle, at least one of the area coordinates is zero, while the
    //! rest lie in the closed interval [0,1].
    class barytriangle2d
    {
    public:

        barytriangle2d( 
            const point2_t v0, 
            const point2_t v1, 
            const point2_t v2 );

        barytriangle2d( 
            float x0, 
            float y0, 
            float x1,
            float y1, 
            float x2, 
            float y2 );

        void get_barycentric_coords( 
            float x, 
            float y, 
            point3_t coord ) const;

        bool point_is_outside( 
            float x, 
            float y ) const;

    private:

        float x3_;
        float y3_;
        float y2_y3_;
        float x1_x3_;
        float x3_x2_;
        float y3_y1_;
        float divider_; // This is the pre-calculated : (y2 - y3)(x1 - x3) + (x3 - x2)(y1 - y3)
    };

    //-------------------------------------------------------------------------

    inline void barytriangle2d::get_barycentric_coords( float x, float y, point3_t coord ) const
    {
        coord[0] = ( ( ( x - x3_ ) * y2_y3_ ) + ( ( y - y3_ ) * x3_x2_ ) ) / divider_;
        coord[1] = ( ( ( x - x3_ ) * y3_y1_ ) + ( ( y - y3_ ) * x1_x3_ ) ) / divider_;
        coord[2] = 1.0f - coord[0] - coord[1];
    }
}

#endif
