#ifndef solosnake_sphere_geometry_hpp
#define solosnake_sphere_geometry_hpp

#include "solosnake/indexed_triangles.hpp"

namespace solosnake
{
    //! Creates several indexed triangle models of spheres of increasing ,
    //! tessellation of a specified radius.
    class sphere_geometry
    {
    public:
        struct index_details
        {
            size_t offset;
            size_t indice_count;
        };

        explicit sphere_geometry( float radius );

        sphere_geometry( sphere_geometry&& );

        sphere_geometry& operator=( sphere_geometry && );

        const index_details sphere_details( size_t n ) const;

        const indexed_triangles& sphere_models() const;

        size_t sphere_index_count( size_t n ) const;

        const unsigned short* sphere_indices( size_t n ) const;

        const float* spheres_vertices() const;

        size_t spheres_vertex_count() const;

        size_t spheres_count() const;

    private:
        std::vector<index_details> sphereIndices_;
        indexed_triangles models_;
    };

    //-------------------------------------------------------------------------

    inline const sphere_geometry::index_details sphere_geometry::sphere_details( size_t n ) const
    {
        return sphereIndices_.at( n );
    }

    inline const indexed_triangles& sphere_geometry::sphere_models() const
    {
        return models_;
    }

    inline size_t sphere_geometry::sphere_index_count( size_t n ) const
    {
        return sphereIndices_.empty() ? 0 : sphere_details( n ).indice_count;
    }

    inline const unsigned short* sphere_geometry::sphere_indices( size_t n ) const
    {
        return models_.indices() + sphere_details( n ).offset;
    }

    inline const float* sphere_geometry::spheres_vertices() const
    {
        return models_.vertices();
    }

    inline size_t sphere_geometry::spheres_vertex_count() const
    {
        return models_.vertex_count();
    }

    inline size_t sphere_geometry::spheres_count() const
    {
        return sphereIndices_.size();
    }
}

#endif
