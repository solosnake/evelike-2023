#ifndef solosnake_indexed_triangles_hpp
#define solosnake_indexed_triangles_hpp

#include <cstddef>
#include <vector>

namespace solosnake
{
    //! A class not really intended as a final production asset, but rather a
    //! useful intermediate tool class. This is effectively a mesh without
    //! texture coordinates, and it can be used to build meshes with.
    class indexed_triangles
    {
    public:

        indexed_triangles();

        indexed_triangles( indexed_triangles&& other );

        indexed_triangles& operator=( indexed_triangles && other );

        void push_tri( unsigned short i0, unsigned short i1, unsigned short i2 );

        void place_vertex_at( float x, float y, float z, size_t n );

        size_t vertex_count() const;

        size_t index_count() const;

        size_t triangle_count() const;

        unsigned short index( size_t n ) const;

        const float* indexed_vertex( unsigned short i ) const;

        const unsigned short* indices() const;

        const float* vertices() const;

    private:

        std::vector<unsigned short> indices_;
        std::vector<float>          vertices_;
    };

    //-------------------------------------------------------------------------

    inline size_t indexed_triangles::vertex_count() const
    {
        return vertices_.size() / 3;
    }

    inline size_t indexed_triangles::index_count() const
    {
        return indices_.size();
    }

    inline size_t indexed_triangles::triangle_count() const
    {
        return indices_.size() / 3;
    }

    inline unsigned short indexed_triangles::index( size_t n ) const
    {
        return indices_.at( n );
    }

    inline const float* indexed_triangles::indexed_vertex( unsigned short i ) const
    {
        return &vertices_.at( i * 3 );
    }

    inline const unsigned short* indexed_triangles::indices() const
    {
        return indices_.data();
    }

    inline const float* indexed_triangles::vertices() const
    {
        return vertices_.data();
    }

    inline indexed_triangles::indexed_triangles() : indices_(), vertices_()
    {
    }
}

#endif
