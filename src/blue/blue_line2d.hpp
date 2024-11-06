#ifndef blue_line2d_hpp
#define blue_line2d_hpp

#include <cmath>
#include "solosnake/blue/blue_point2d.hpp"
#include "solosnake/blue/blue_radians.hpp"

namespace blue
{
    /// Line in 2D.
    class Line2d
    {
    public:

        /// Default line is y = 0.
        Line2d() = default;

        /// The line is undefined if a and b are the same points.
        Line2d( const Point2d& a,
                const Point2d& b ) noexcept;

        /// The line is undefined if p0 and p1 are the same points.
        Line2d( float x0,
                float y0,
                float x1,
                float y1 ) noexcept;

        ///a i the angle made with the x axis anticlockwise.
        Line2d( const Point2d& a,
                const Radians& m ) noexcept;

        const Point2d& point_on_line() const noexcept;

        Radians angle() const noexcept;

        bool operator==( const Line2d& rhs ) const noexcept;
        bool operator!=( const Line2d& rhs ) const noexcept;

    private:
        Point2d point_;
        Radians angle_;
    };
}

#include "solosnake/blue/blue_line2d.inl"
#endif
