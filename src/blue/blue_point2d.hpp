#ifndef blue_point2d_hpp
#define blue_point2d_hpp

#include <cstdint>

namespace blue
{
    struct Point2d
    {
        Point2d() = default;

        Point2d( float x, float y ) noexcept : xy{ x, y }
        {
        }

        constexpr float operator [] (std::size_t i) const noexcept
        {
            return xy[i];
        }

        constexpr float& operator [] (std::size_t i) noexcept
        {
            return xy[i];
        }

        bool operator == (const Point2d& rhs) const noexcept
        {
            return xy[0] == rhs.xy[0] && xy[1] == rhs.xy[1];
        }

        bool operator != (const Point2d& rhs) const noexcept
        {
            return xy[0] != rhs.xy[0] || xy[1] != rhs.xy[1];
        }

        float xy[2] = {};
    };
}

#endif
