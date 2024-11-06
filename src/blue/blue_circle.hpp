#ifndef blue_circle_hpp
#define blue_circle_hpp

#include <cassert>
#include "solosnake/blue/blue_point2d.hpp"

namespace blue
{
    class Circle
    {
    public:

        Circle() = default;

        Circle( const Point2d& c, const float r ) : centre_( c ), radius_( r )
        {
            assert( r > 0.0f );
        }

        Circle( const float x, const float y, const float r ) : centre_( x, y ), radius_( r )
        {
            assert( r > 0.0f );
        }

        const Point2d& centre() const noexcept
        {
            return centre_;
        }

        float radius() const noexcept
        {
            return radius_;
        }

        bool operator==( const Circle& rhs ) const noexcept
        {
            return centre_ == rhs.centre_ && radius_ == rhs.radius_;
        }

        bool operator!=( const Circle& rhs ) const noexcept
        {
            return centre_ != rhs.centre_ || radius_ != rhs.radius_;
        }

    private:

        Point2d centre_ = {};
        float   radius_ = 1.0f;
    };
}

#endif