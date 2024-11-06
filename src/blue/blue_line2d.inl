#include "solosnake/blue/blue_line2d.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"

namespace blue
{
    /// The line is undefined if p0 and p1 are the same points.
    inline Line2d::Line2d( const Point2d& a,
                           const Point2d& b ) noexcept
        : point_( a )
        , angle_( a[0] == b[0]
            ? Radians(0.50f * BLUE_PI)
            : Radians( std::atan2( b[1] - a[1], b[0] - a[0] ) ) )
    {
    }

    /// The line is undefined if p0 and p1 are the same points.
    inline Line2d::Line2d( const float x0,
                           const float y0,
                           const float x1,
                           const float y1 ) noexcept
        : point_( x0, y0 )
        , angle_( x0 == x1
            ? Radians(0.50f * BLUE_PI)
            : Radians( std::atan2( y1 - y0, x1 - x0 ) ) )
    {
    }

    inline Line2d::Line2d( const Point2d& p, const Radians& a ) noexcept
        : point_( p ), angle_( a )
    {
    }

    inline bool Line2d::operator==( const Line2d& rhs ) const noexcept
    {
        return point_ == rhs.point_ && angle_ == rhs.angle_;
    }

    inline bool Line2d::operator!=( const Line2d& rhs ) const noexcept
    {
        return point_ != rhs.point_ || angle_ != rhs.angle_;
    }

    inline const Point2d& Line2d::point_on_line() const noexcept
    {
        return point_;
    }

    inline Radians Line2d::angle() const noexcept
    {
        return angle_;
    }
}
