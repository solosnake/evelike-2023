#include "solosnake/foreachtriangle.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/throw.hpp"
#include <cassert>

namespace solosnake
{
    namespace
    {
        void iterate_triangles( const imeshgeometry& geom,
                                const imeshtexturecoords& coords,
                                for_each_triangle::callback& cb )
        {
            const unsigned int n = geom.index_count();

            if( 0 == n )
            {
                return;
            }

            const float* v = geom.vertices();
            const float* uv = coords.texture_coords();
            const unsigned short* indices = geom.indices();
            unsigned short idcounter = 0;

            assert( 0 == n % 3 );
            assert( v );
            assert( uv );
            assert( indices );

            for( unsigned int i = 0; i < n; i += 3 )
            {
                for_each_triangle::triangle t;

                const float* v0 = v + ( 3 * indices[i] );
                const float* uv0 = uv + ( 2 * indices[i] );

                t.verts[0].x = v0[0];
                t.verts[0].y = v0[1];
                t.verts[0].z = v0[2];

                t.verts[0].u = uv0[0];
                t.verts[0].v = uv0[1];

                t.indices[0] = indices[i];

                const float* v1 = v + ( 3 * indices[1 + i] );
                const float* uv1 = uv + ( 2 * indices[1 + i] );

                t.verts[1].x = v1[0];
                t.verts[1].y = v1[1];
                t.verts[1].z = v1[2];

                t.verts[1].u = uv1[0];
                t.verts[1].v = uv1[1];

                t.indices[1] = indices[1 + i];

                const float* v2 = v + ( 3 * indices[2 + i] );
                const float* uv2 = uv + ( 2 * indices[2 + i] );

                t.verts[2].x = v2[0];
                t.verts[2].y = v2[1];
                t.verts[2].z = v2[2];

                t.verts[2].u = uv2[0];
                t.verts[2].v = uv2[1];

                t.indices[2] = indices[2 + i];

                t.id = idcounter++;

                // Call user's method with triangle and mesh:
                ( cb )( geom, t );
            }
        }

        //! GL_TRIANGLE_STRIP Draws a series of triangles (three-sided polygons)
        //! using vertices v0, v1, v2, then v2, v1, v3 (note the order), then v2,
        //! v3, v4, and so on. The ordering is to ensure that the triangles are
        //! all drawn with the same orientation so that the strip can correctly
        //! form part of a surface.
        //!
        //! v0, v1, v2, v3, v4, ...
        //!
        //! 012
        //! 213
        //! 234
        //!
        //! Also
        //! Here is an example. Let's say you have an index array as follows:
        //!
        //! { 0 1 2 3 65535 2 3 4 5 }
        //!
        //! If you render this as a triangle strip normally, you get 7 triangles.
        //! If you render it with glPrimitiveRestartIndex(65535) and the primitive
        //! restart enabled, then you will get 4 triangles:
        //!
        //! {0 1 2}, {2 1 3}, {2 3 4}, {4 3 5}.
        //!
        //! Primitive restart works with any of the versions of these functions.
        //! http://www.opengl.org/wiki/Primitives#Primitive_Restart
        //! http://www.opengl.org/wiki/Vertex_Specification#Primitive_Restart
        //!
        void iterate_strips( const imeshgeometry&,
                             const imeshtexturecoords&,
                             for_each_triangle::callback& )
        {
            /*
                const float* v = geom.vertices();
                const float* uv = coords.texture_coords();
                const unsigned short* indices = geom.indices();
                unsigned int n = geom.index_count();
               */
            ss_throw( "NOT IMPLEMENTED" );
        }
    }

    //! Calls the user's callback object once per model triangle.
    void for_each_triangle::iterate( const imeshgeometry& geom,
                                     const imeshtexturecoords& coords,
                                     for_each_triangle::callback cb )
    {
        switch( geom.index_mode() )
        {
            case imeshgeometry::TrianglesMode:
                iterate_triangles( geom, coords, cb );
                break;

            case imeshgeometry::TriangleStripsMode:
                iterate_strips( geom, coords, cb );
                break;

            default:
                assert( ! "Unhandled model type." );
                ss_throw( "Unhandled model type." );
                break;
        }
    }
}
