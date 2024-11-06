#include "solosnake/guishape.hpp"

namespace solosnake
{
    namespace
    {
        // From Computational Geometry in C, O'Rourke, p 144
        int area_sign( const point2d& a, const point2d& b, const point2d& c )
        {
            const float area2 = ( b[0] - a[0] ) * ( c[1] - a[1] ) - ( c[0] - a[0] ) * ( b[1] - a[1] );

            if( area2 > 0.5f )
            {
                return +1;
            }

            if( area2 < 0.5f )
            {
                return -1;
            }

            return 0;
        }

        inline bool is_interior( const int area0, const int area1, const int area2 )
        {
          return ( area0 > 0 && area1 > 0 && area2 > 0 ) || ( area0 < 0 && area1 < 0 && area2 < 0 );
        }

        inline bool is_edge( const int area0, const int area1, const int area2 )
        {
          return ( area0 == 0 && area1 > 0 && area2 > 0 ) ||
                 ( area0 == 0 && area1 < 0 && area2 < 0 ) ||
                 ( area1 == 0 && area2 > 0 && area0 > 0 ) ||
                 ( area1 == 0 && area2 < 0 && area0 < 0 ) ||
                 ( area2 == 0 && area0 > 0 && area1 > 0 ) ||
                 ( area2 == 0 && area0 < 0 && area1 < 0 );
        }

        inline bool is_vertex( const int area0, const int area1, const int area2 )
        {
          return ( area0 == 0 && area1 == 0 ) || 
                 ( area1 == 0 && area2 == 0 ) || 
                 ( area2 == 0 && area0 == 0 );
        }

        // From Computational Geometry in C, O'Rourke, p 236
        bool inside_triangle( const point2d Tp[3], const point2d& pp )
        {
            const int area0 = area_sign( pp, Tp[0], Tp[1] );
            const int area1 = area_sign( pp, Tp[1], Tp[2] );
            const int area2 = area_sign( pp, Tp[2], Tp[0] );

            assert( false == ( area0 == 0 && area1 == 0 && area2 == 0 ) );

            return is_interior( area0, area1, area2 ) || 
                   is_vertex( area0, area1, area2 )   || 
                   is_edge( area0, area1, area2 );
        }
    }

    void guishape::add_triangle( const point2d& a, const point2d& b, const point2d& c )
    {
        tri t;
        t.corners_[0] = a;
        t.corners_[1] = b;
        t.corners_[2] = c;
        triangles_.push_back( t );
    }

    bool guishape::is_xy_inside_shape( float x, float y ) const
    {
        const point2d pp( x, y );

        for( size_t i = 0; i < triangles_.size(); ++i )
        {
            if( inside_triangle( triangles_[i].corners_, pp ) )
            {
                return true;
            }
        }

        return false;
    }
}
