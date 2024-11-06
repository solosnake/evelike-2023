#include "solosnake/sphere_geometry.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/icosahedron.hpp"
#include "solosnake/point.hpp"
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <map>

namespace solosnake
{

    namespace
    {
        //! Helper class representing a vertex midway between two other indexed
        // vertices.
        class pseudo_vert
        {
        public:

            pseudo_vert( const unsigned short indexA,
                         const unsigned short indexB )
                : indices_( std::make_pair( std::min( indexA, indexB ),
                                            std::max( indexA, indexB ) ) )
            {
                assert( indexA != indexB );
            }

            unsigned short low() const
            {
                return indices_.first;
            }

            unsigned short high() const
            {
                return indices_.second;
            }

            bool operator<( const pseudo_vert& rhs ) const
            {
                return indices_ < rhs.indices_;
            }

        private:

            std::pair<unsigned short, unsigned short> indices_;
        };

        //! Adds one level of triangles to the model and ensures all its
        //! vertices are 'vertexDistance' away from the origin. This of course
        // assumes
        //! that m is an approximation of a sphere centred at the origin.
        void tesselate_sphere( indexed_triangles& m, float vertexDistance )
        {
            const size_t nTris = m.triangle_count();

            unsigned short nextIndex = static_cast<unsigned short>( m.vertex_count() );

            std::map<pseudo_vert, unsigned short> fakeVerts;

            // Lambda taking an edge's end indices and insert a (possibly) new
            // pseudo vertex representing its midpoint into the collection.
            // Returns its intended index in the final vertex buffer.
            auto add_vert = [&]( unsigned short iA, unsigned short iB )->unsigned short
            {
                if( nextIndex == std::numeric_limits<unsigned short>::max() )
                {
                    ss_throw( "Cannot tesselate any further, max index reached." );
                }

                pseudo_vert newVert( iA, iB );

                auto v = fakeVerts.find( newVert );

                if( v == fakeVerts.end() )
                {
                    fakeVerts[newVert] = nextIndex;
                    return nextIndex++; // Increment but return previous value.
                }

                return v->second;
            };

            // Subdivide each triangle into 4 new triangles.
            for( size_t i = 0; i < nTris; ++i )
            {
                auto i0 = m.index( 3 * i + 0 );
                auto i1 = m.index( 3 * i + 1 );
                auto i2 = m.index( 3 * i + 2 );

                auto i01 = add_vert( i0, i1 );
                auto i12 = add_vert( i1, i2 );
                auto i02 = add_vert( i0, i2 );

                // Push back new triangles:
                m.push_tri( i0, i01, i02 );
                m.push_tri( i02, i01, i12 );
                m.push_tri( i01, i1, i12 );
                m.push_tri( i12, i2, i02 );
            }

            // Now convert pseudo verts to actual verts and
            // also convert to a unit distance:
            std::for_each( fakeVerts.cbegin(),
                           fakeVerts.cend(),
                           [&]( const std::map<pseudo_vert, unsigned short>::value_type & v )
            {
                // Find endpoints of original edge:
                point3_t vert;
                midpoint3( m.indexed_vertex( v.first.low() ), m.indexed_vertex( v.first.high() ), vert );

                // Convert to vector on unit sphere:
                normalise3( vert );

                // Change sphere radius:
                mul3( vert, vertexDistance );

                // Insert vertex.
                m.place_vertex_at( vert[0], vert[1], vert[2], v.second );
            } );
        }
    }

    sphere_geometry::sphere_geometry( float radius )
    {
        if( radius <= 0.0f )
        {
            ss_throw( "sphere_geometry cannot be created with radius lte zero." );
        }

        // Tessellations of the sphere beyond this require indices greater than
        // the max unsigned short.
        const size_t SS_TESSELATION_MAX = 6;

        models_ = make_icosahedron( radius );

        // Find the length of the icosahedron vertices and use this as the vertex
        // length
        // of the tessellated spheres:
        const float sideLen = length3( models_.vertices() );

        sphere_geometry::index_details details;
        details.offset = 0;
        details.indice_count = models_.index_count();

        // Sphere 0.
        sphereIndices_.push_back( details );

        // Add spheres beyond 0th sphere:
        for( size_t i = 1; i < SS_TESSELATION_MAX; ++i )
        {
            tesselate_sphere( models_, sideLen );

            details.offset = details.indice_count;
            details.indice_count = models_.index_count() - details.offset;

            sphereIndices_.push_back( details );
        }
    }

    sphere_geometry::sphere_geometry( sphere_geometry&& other )
    {
        *this = std::move( other );
    }

    sphere_geometry& sphere_geometry::operator=( sphere_geometry && other )
    {
        if( this != &other )
        {
            sphereIndices_ = std::move( other.sphereIndices_ );
            models_ = std::move( other.models_ );
        }

        return *this;
    }
}
