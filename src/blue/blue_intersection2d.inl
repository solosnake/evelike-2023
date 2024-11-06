#include "solosnake/blue/blue_intersection2d.hpp"
#include <cassert>

namespace blue
{
    inline Intersection2d::Intersection2d( float x, float y ) noexcept
        : intersections_count_( 1u )
    {
        intersections_[0][0] = x;
        intersections_[0][1] = y;
        intersections_[1][0] = x;
        intersections_[1][1] = y;
    }

    inline Intersection2d::Intersection2d( float x1, float y1,
                                            float x2, float y2 ) noexcept
        : intersections_count_( ( x1 == x2 && y1 == y2 ) ? 1u : 2u )
    {
        intersections_[0][0] = x1;
        intersections_[0][1] = y1;
        intersections_[1][0] = x2;
        intersections_[1][1] = y2;
    }

    inline bool Intersection2d::empty() const noexcept
    {
        return 0 == intersections_count_;
    }

    inline unsigned int Intersection2d::size() const noexcept
    {
        return intersections_count_;
    }

    inline const Point2d* Intersection2d::cbegin() const noexcept
    {
        return intersections_;
    }

    inline const Point2d* Intersection2d::cend() const noexcept
    {
        return intersections_count_ + intersections_;
    }

    inline Point2d* Intersection2d::begin() noexcept
    {
        return intersections_;
    }

    inline Point2d* Intersection2d::end() noexcept
    {
        return intersections_count_ + intersections_;
    }

    inline const Point2d& Intersection2d::operator[]( unsigned int i ) const noexcept
    {
        assert( intersections_count_ > 0 && i < 2 );
        return intersections_[i];
    }

    inline Intersection2d::operator bool() const noexcept
    {
        return intersections_count_ > 0u;
    }
}
