#include <cmath>
#include "solosnake/icosahedron.hpp"
#include "solosnake/indexed_triangles.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    //! Creates an icosahedron to ENCLOSE a sphere of the given radius.
    indexed_triangles make_icosahedron( float radius )
    {
        if( radius <= 0.0f )
        {
            ss_throw( "make_icosahedron must be created with radius greater than zero." );
        }

        indexed_triangles model;

        // http://en.wikipedia.org/wiki/Icosahedron
        // Forms an icosahedron of edge length 2.
        static const float t = 1.6180339887498948482045868343657f; // (1 + sqrt(5)) / 2 // constexpr
        static const float inscribedSphereRadius = 2.0f * 0.7557613141f;

        const float t0 = radius * t / inscribedSphereRadius;
        const float ti = radius / inscribedSphereRadius;

        model.place_vertex_at( t0, 0.0f, ti, 0 );
        model.place_vertex_at( t0, 0.0f, -ti, 1 );
        model.place_vertex_at( -t0, 0.0f, ti, 2 );
        model.place_vertex_at( -t0, 0.0f, -ti, 3 );

        model.place_vertex_at( 0.0f, ti, t0, 4 );
        model.place_vertex_at( 0.0f, ti, -t0, 5 );
        model.place_vertex_at( 0.0f, -ti, t0, 6 );
        model.place_vertex_at( 0.0f, -ti, -t0, 7 );

        model.place_vertex_at( ti, t0, 0.0f, 8 );
        model.place_vertex_at( ti, -t0, 0.0f, 9 );
        model.place_vertex_at( -ti, t0, 0.0f, 10 );
        model.place_vertex_at( -ti, -t0, 0.0f, 11 );

        model.push_tri( 10, 4, 8 );
        model.push_tri( 8, 5, 10 );
        model.push_tri( 10, 5, 3 );
        model.push_tri( 3, 2, 10 );
        model.push_tri( 10, 2, 4 );

        model.push_tri( 9, 0, 6 );
        model.push_tri( 6, 11, 9 );
        model.push_tri( 9, 11, 7 );
        model.push_tri( 7, 1, 9 );
        model.push_tri( 9, 1, 0 );

        model.push_tri( 0, 4, 6 );
        model.push_tri( 0, 8, 4 );
        model.push_tri( 4, 2, 6 );
        model.push_tri( 6, 2, 11 );
        model.push_tri( 11, 2, 3 );

        model.push_tri( 3, 7, 11 );
        model.push_tri( 3, 5, 7 );
        model.push_tri( 7, 5, 1 );
        model.push_tri( 1, 5, 8 );
        model.push_tri( 8, 0, 1 );

        return model;
    }
}
