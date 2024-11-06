#ifndef solosnake_polygonshape_hpp
#define solosnake_polygonshape_hpp

#include <vector>
#include "solosnake/ishape.hpp"
#include "solosnake/barytriangle.hpp"

namespace solosnake
{
    //! A 2D polygon shape composed of triangles. The winding order of the triangles is
    //! not important, nor is overlapping or singular triangles.
    class polygonshape : public ishape
    {
    public:

        bool is_xy_inside_shape( float x, float y ) const override;

        size_t triangles_count() const;

        void add_triangle( float x0, float y0, float x1, float y1, float z0, float z1 );

        void add_triangle( const point2_t p0, const point2_t p1, const point2_t p2 );

    private:

        std::vector<barytriangle2d> triangles_;
    };
}

#endif
