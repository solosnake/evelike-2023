#ifndef solosnake_sphere_hpp
#define solosnake_sphere_hpp

#include <utility>
#include "solosnake/dimension.hpp"
#include "solosnake/point.hpp"

namespace solosnake
{
    class sphere
    {
    public:
        //! Zero dimension sphere at 0,0,0.
        sphere();

        sphere( const point3d& pos, float radius );

        //! Returns true if @a p is inside box.
        bool contains( const point3d& p ) const;

        //! Returns the nearest point in or on the sphere surface to @a p.
        point3d nearest_point_to( const point3d& p ) const;

        //! Returns mid point in sphere.
        point3d centre() const;

        float radius() const;

        //! Returns a copy of this sphere shrunk towards its centre by this amount.
        sphere shrunk_by( float amount ) const;

    private:
        point3d centre_;
        float radius_;
    };

    //-------------------------------------------------------------------------

    inline point3d sphere::centre() const
    {
        return centre_;
    }

    inline float sphere::radius() const
    {
        return radius_;
    }

    inline sphere sphere::shrunk_by( float amount ) const
    {
        return sphere( centre_, std::max( 0.0f, radius_ - amount ) );
    }
}

#endif
