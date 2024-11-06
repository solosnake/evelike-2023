#ifndef blue_intersection2d_hpp
#define blue_intersection2d_hpp

#include "solosnake/blue/blue_point2d.hpp"

namespace blue
{
    /// Represents the results of a 2D intersection test. Can return 0, 1 or 2
    /// points. When there is an intersection, even if it is only 1, both points
    /// are valid to read: the single intersection case will return the same
    /// point.
    class Intersection2d
    {
    public:

        /// No intersections.
        Intersection2d() = default;

        /// One intersection.
        Intersection2d( float x, float y ) noexcept;

        /// Two intersections (reduces to one if x1 == x2 and y1 == y2).
        Intersection2d( float x1, float y1, float x2, float y2 ) noexcept;

        bool            empty() const noexcept;

        unsigned int    size() const noexcept;

        const Point2d*  cbegin() const noexcept;

        const Point2d*  cend() const noexcept;

        Point2d*        begin() noexcept;

        Point2d*        end() noexcept;

        const Point2d&  operator[]( unsigned int i ) const noexcept;

        /// Returns true if there are intersections.
        operator bool() const noexcept;

    private:

        unsigned int    intersections_count_ = 0u;
        Point2d         intersections_[2]    = {};
    };
}

#include "solosnake/blue/blue_intersection2d.inl"
#endif
