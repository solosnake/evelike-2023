#include "solosnake/indexed_triangles.hpp"

namespace solosnake
{
    indexed_triangles::indexed_triangles( indexed_triangles&& other )
    {
        *this = std::move( other );
    }

    indexed_triangles& indexed_triangles::operator=( indexed_triangles && other )
    {
        if( this != &other )
        {
            indices_ = std::move( other.indices_ );
            vertices_ = std::move( other.vertices_ );
        }

        return *this;
    }

    void indexed_triangles::push_tri( unsigned short i0, unsigned short i1, unsigned short i2 )
    {
        indices_.push_back( i0 );
        indices_.push_back( i1 );
        indices_.push_back( i2 );
    }

    void indexed_triangles::place_vertex_at( float x, float y, float z, size_t n )
    {
        if( 3 * n + 3 > vertices_.size() )
        {
            vertices_.resize( 3 * n + 3 );
        }

        vertices_[n * 3 + 0] = x;
        vertices_[n * 3 + 1] = y;
        vertices_[n * 3 + 2] = z;
    }
}
