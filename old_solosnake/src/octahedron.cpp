#include "solosnake/indexed_triangles.hpp"
#include "solosnake/octahedron.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    indexed_triangles make_octahedron( float radius )
    {
        if( radius <= 0.0f )
        {
            ss_throw( "octahedron must be created with radius greater than zero." );
        }

        indexed_triangles model;

        // Sides
        model.place_vertex_at( radius, 0.0f, 0.0f, 0 );
        model.place_vertex_at( 0.0f, 0.0f, -radius, 1 );
        model.place_vertex_at( -radius, 0.0f, 0.0f, 2 );
        model.place_vertex_at( 0.0f, 0.0f, radius, 3 );

        // Top & Bottom
        model.place_vertex_at( 0.0f, radius, 0.0f, 4 );
        model.place_vertex_at( 0.0f, -radius, 0.0f, 5 );

        // Top
        model.push_tri( 0, 1, 4 );
        model.push_tri( 1, 2, 4 );
        model.push_tri( 2, 3, 4 );
        model.push_tri( 3, 0, 4 );

        // Bottom
        model.push_tri( 1, 0, 5 );
        model.push_tri( 2, 1, 5 );
        model.push_tri( 3, 2, 5 );
        model.push_tri( 0, 3, 5 );

        return model;
    }
}
