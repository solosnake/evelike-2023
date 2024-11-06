#include <map>
#include <memory>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/foreachtriangle.hpp"
#include "solosnake/image.hpp"
#include "solosnake/make_normalmap.hpp"
#include "solosnake/point.hpp"

namespace solosnake
{
    namespace
    {
        void get_tri_normal( const for_each_triangle::triangle& tri, float* n )
        {
            float v0[3] = { tri.verts[1].x - tri.verts[0].x, tri.verts[1].y - tri.verts[0].y,
                            tri.verts[1].z - tri.verts[0].z
                          };

            float v1[3] = { tri.verts[2].x - tri.verts[0].x, tri.verts[2].y - tri.verts[0].y,
                            tri.verts[2].z - tri.verts[0].z
                          };

            cross3( v0, v1, n );
            normalise3( n );
        }

        bgra normal_to_colour( const float* n )
        {
            return bgra( static_cast<unsigned char>( ( n[2] + 1.0f ) * 127.5f ), // Blue is  Z
                         static_cast<unsigned char>( ( n[1] + 1.0f ) * 127.5f ), // Green is Y
                         static_cast<unsigned char>( ( n[0] + 1.0f ) * 127.5f ), // Red is   X
                         0u );
        }

        bgra normal_to_colour( const float* n, unsigned char a )
        {
            return bgra( static_cast<unsigned char>( ( n[2] + 1.0f ) * 127.5f ), // Blue is  Z
                         static_cast<unsigned char>( ( n[1] + 1.0f ) * 127.5f ), // Green is Y
                         static_cast<unsigned char>( ( n[0] + 1.0f ) * 127.5f ), // Red is   X
                         a );
        }

        bgra get_normal_colour( const for_each_triangle::triangle& tri )
        {
            float n[3];
            get_tri_normal( tri, n );
            return normal_to_colour( n );
        }

        void add_to_flat_normalmap( image& normalmap,
                                    const imeshgeometry& msh,
                                    const imeshtexturecoords& tex,
                                    unsigned char a )
        {
            auto f = [&]( const imeshgeometry&, const for_each_triangle::triangle & tri )
            {
                const bgra normalColour( get_normal_colour( tri ).blue_green_red(), a );

                normalmap.draw_triangle( tri.verts[0].u,
                                         tri.verts[0].v,
                                         normalColour,
                                         tri.verts[1].u,
                                         tri.verts[1].v,
                                         normalColour,
                                         tri.verts[2].u,
                                         tri.verts[2].v,
                                         normalColour );
            };

            for_each_triangle::iterate( msh, tex, f );
        }

        class nrmlaccum
        {
        public:

            nrmlaccum() : x( 0.0f ), y( 0.0f ), z( 0.0f ), n( 0.0 )
            {
            }

            void operator+=( const float v[3] )
            {
                x += v[0];
                y += v[1];
                z += v[2];
                ++n;
            }

            float* mean( float m[3] )
            {
                assert( n > 0.0f );
                m[0] = x / n;
                m[1] = y / n;
                m[2] = z / n;
                normalise3( m );
                return m;
            }

        private:
            float x, y, z, n;
        };

        void add_to_smooth_normalmap( image& normalmap,
                                      const imeshgeometry& msh,
                                      const imeshtexturecoords& tex,
                                      unsigned char a )
        {

            std::vector<nrmlaccum> nrmlssummed( msh.index_count() );

            auto accum = [&]( const imeshgeometry&, const for_each_triangle::triangle & tri )
            {
                float n[3];
                get_tri_normal( tri, n );
                nrmlssummed.at( tri.indices[0] ) += n;
                nrmlssummed.at( tri.indices[1] ) += n;
                nrmlssummed.at( tri.indices[2] ) += n;
            };

            for_each_triangle::iterate( msh, tex, accum );

            auto drawsmoothed = [&]( const imeshgeometry&, const for_each_triangle::triangle & tri )
            {
                float m[3][3];

                bgra normalColour[3] = { normal_to_colour( nrmlssummed[tri.indices[0]].mean( m[0] ), a ),
                                         normal_to_colour( nrmlssummed[tri.indices[1]].mean( m[1] ), a ),
                                         normal_to_colour( nrmlssummed[tri.indices[2]].mean( m[2] ), a )
                                       };

                normalmap.draw_triangle( tri.verts[0].u,
                                         tri.verts[0].v,
                                         normalColour[0],
                                         tri.verts[1].u,
                                         tri.verts[1].v,
                                         normalColour[1],
                                         tri.verts[2].u,
                                         tri.verts[2].v,
                                         normalColour[2] );
            };

            for_each_triangle::iterate( msh, tex, drawsmoothed );
        }
    }

    //-------------------------------------------------------------------------

    void add_to_normalmap( image& normalmap,
                           const imeshgeometry& msh,
                           const imeshtexturecoords& tex,
                           bool smooth,
                           unsigned char alpha )
    {
        return smooth ? add_to_smooth_normalmap( normalmap, msh, tex, alpha )
               : add_to_flat_normalmap( normalmap, msh, tex, alpha );
    }

    image make_normalmap( const imeshgeometry& msh,
                          const imeshtexturecoords& tex,
                          unsigned int w,
                          unsigned int h,
                          bool smooth,
                          unsigned char alpha )
    {
        image normalmap( static_cast<unsigned int>( w ), static_cast<unsigned int>( h ), image::format_bgra );

        // Clear to mid grey (0,0,0,0)
        normalmap.clear_to_bgra( 127, 127, 127, 127 );

        add_to_normalmap( normalmap, msh, tex, smooth, alpha );

        return normalmap;
    }
}
