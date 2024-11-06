#ifndef blue_point3d_hpp
#define blue_point3d_hpp

#include <cassert>
#include <cmath>

namespace blue
{
    struct Point3d
    {
        Point3d() = default;

        Point3d( float x, float y, float z ) noexcept : xyz{x, y, z}
        {
        }

        Point3d& operator-=( const Point3d& rhs ) noexcept
        {
            xyz[0] -= rhs.xyz[0];
            xyz[1] -= rhs.xyz[1];
            xyz[2] -= rhs.xyz[2];
            return *this;
        }

        Point3d operator+( const Point3d& rhs ) const noexcept
        {
            return Point3d( xyz[0] + rhs.xyz[0], xyz[1] + rhs.xyz[1], xyz[2] + rhs.xyz[2] );
        }

        bool operator == (const Point3d& rhs) const noexcept
        {
            return xyz[0] == rhs.xyz[0]
                && xyz[1] == rhs.xyz[1]
                && xyz[2] == rhs.xyz[2]
                ;
        }

        bool operator != (const Point3d& rhs) const noexcept
        {
            return xyz[0] != rhs.xyz[0]
                || xyz[1] != rhs.xyz[1]
                || xyz[2] != rhs.xyz[2]
                ;
        }

        float normalize() noexcept
        {
            const float f = std::sqrt( ( xyz[0] * xyz[0] ) + ( xyz[1] * xyz[1] ) + ( xyz[2] * xyz[2] ) );
            assert( f > 0.0001f );

            xyz[0] /= f;
            xyz[1] /= f;
            xyz[2] /= f;

            return f;
        }

        float xyz[3] = {};
    };
}

#endif
