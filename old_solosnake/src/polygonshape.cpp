#include "solosnake/polygonshape.hpp"
#include "solosnake/logging.hpp"

namespace solosnake
{
    bool polygonshape::is_xy_inside_shape( float x, float y ) const
    {
        ss_dbg( "Polygon query ", x, " ", y );
        for( size_t i = 0u; i < triangles_.size(); ++i )
        {
            if( !triangles_[i].point_is_outside( x, y ) )
            {
                return true;
            }
        }

        return false;
    }

    size_t polygonshape::triangles_count() const
    {
        return triangles_.size();
    }

    void polygonshape::add_triangle( float x0, float y0, float x1, float y1, float z0, float z1 )
    {
        triangles_.push_back( barytriangle2d( x0, y0, x1, y1, z0, z1 ) );
    }

    void polygonshape::add_triangle( const point2_t p0, const point2_t p1, const point2_t p2 )
    {
        triangles_.push_back( barytriangle2d( p0, p1, p2 ) );
    }
}
