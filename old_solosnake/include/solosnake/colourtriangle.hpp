#ifndef solosnake_colourtriangle_hpp
#define solosnake_colourtriangle_hpp

#include "solosnake/bgra.hpp"
#include "solosnake/point.hpp"
#include "solosnake/barytriangle.hpp"

namespace solosnake
{
    //! Uses barycentric coordinates to interpolate a smoothed
    //! colour inside (and outside) a given 2D triangle.
    class colourtriangle2d
    {
    public:

        colourtriangle2d(
            const point2_t v0,
            const bgra& c0,
            const point2_t v1,
            const bgra& c1,
            const point2_t v2,
            const bgra& c2 );

        colourtriangle2d(
            float x0,
            float y0,
            const bgra& c0,
            float x1,
            float y1,
            const bgra& c1,
            float x2,
            float y2,
            const bgra& c2 );

        bgra get_barycentric_colour( float x, float y ) const;

        bgra get_barycentric_colour( const point2_t p ) const;

    private:

        barytriangle2d  barytri_;
        bgra            c0_;
        bgra            c1_;
        bgra            c2_;
    };

    //-------------------------------------------------------------------------

    inline bgra colourtriangle2d::get_barycentric_colour( const point2_t p ) const
    {
        return get_barycentric_colour( p[0], p[1] );
    }
}

#endif
