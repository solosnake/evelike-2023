#ifndef blue_half_line_3d_hpp
#define blue_half_line_3d_hpp

#include "solosnake/blue/blue_point3d.hpp"
#include "solosnake/blue/blue_unaliased.hpp"

namespace blue
{
    /// A line segment where A -> B and B -> A describe different lines in space.
    class Half_line_3d
    {
    public:

        /// Construct a 3D half line from p0 to p1.
        /// Throws if the line is singularity.
        Half_line_3d( const Point3d& p0, const Point3d& p1 );

        const Point3d& head() const noexcept;

        /// The tail is unit length away from head.
        Point3d tail() const noexcept;

        /// Unit length direction vector from head.
        const Point3d& direction() const noexcept;

        bool operator==( const Half_line_3d& rhs ) const noexcept;

        bool operator!=( const Half_line_3d& rhs ) const noexcept;

    private:

        void normalize();

    private:

        Point3d start_;
        Point3d dir_;
    };

    /// Returns true if the line intersects with the plane Y=y and fills in the
    /// intersection point. Plane parallel lines are considered to not intersect.
    bool find_half_line_y_plane_intersection( const Half_line_3d&,
                                              const float y,
                                              float* unaliased intersectionXYZ );
}

#include "solosnake/blue/blue_half_line_3d.inl"
#endif
