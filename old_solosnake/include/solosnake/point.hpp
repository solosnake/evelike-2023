#ifndef solosnake_point_hpp
#define solosnake_point_hpp

#include <cmath>
#include <cassert>
#include <functional>
#include "solosnake/unaliased.hpp"

namespace solosnake
{
    typedef float point2_t[2];
    typedef float point3_t[3];

    /*
    typedef float point4_t[4];
*/

    struct Point2d
    {
        Point2d() = default;

        explicit Point2d( const float* );

        Point2d( float, float );

        Point2d operator+( const Point2d& rhs ) const
        {
            return Point2d( xy[0] + rhs.xy[0], xy[1] + rhs.xy[1] );
        }

        Point2d& operator-=( const Point2d& rhs )
        {
            xy[0] -= rhs.xy[0];
            xy[1] -= rhs.xy[1];
            return *this;
        }

        Point2d& operator+=( const Point2d& rhs )
        {
            xy[0] += rhs.xy[0];
            xy[1] += rhs.xy[1];
            return *this;
        }

        operator const float* () const
        {
            return xy;
        }

        operator float* ()
        {
            return xy;
        }

        point2_t xy = {};
    };

    struct Point3d
    {
        Point3d() = default;

        explicit Point3d( const float* );

        Point3d( float, float, float );

        Point3d operator+( const Point3d& rhs ) const
        {
            return Point3d( xyz[0] + rhs.xyz[0], xyz[1] + rhs.xyz[1], xyz[2] + rhs.xyz[2] );
        }

        Point3d& operator-=( const Point3d& rhs )
        {
            xyz[0] -= rhs.xyz[0];
            xyz[1] -= rhs.xyz[1];
            xyz[2] -= rhs.xyz[2];
            return *this;
        }

        Point3d& operator+=( const Point3d& rhs )
        {
            xyz[0] += rhs.xyz[0];
            xyz[1] += rhs.xyz[1];
            xyz[2] += rhs.xyz[2];
            return *this;
        }

        operator const float* () const
        {
            return xyz;
        }
        operator float* ()
        {
            return xyz;
        }

        point3_t xyz = {};
    };

/*
    struct Point4d
    {
        Point4d() = default;

        explicit Point4d( const float* );

        Point4d( float, float, float, float );

        Point4d operator+( const Point4d& rhs ) const
        {
            return Point4d( xyzw[0] + rhs.xyzw[0],
                            xyzw[1] + rhs.xyzw[1],
                            xyzw[2] + rhs.xyzw[2],
                            xyzw[3] + rhs.xyzw[3] );
        }

        Point4d& operator-=( const Point4d& rhs )
        {
            xyzw[0] -= rhs.xyzw[0];
            xyzw[1] -= rhs.xyzw[1];
            xyzw[2] -= rhs.xyzw[2];
            xyzw[3] -= rhs.xyzw[3];
            return *this;
        }

        Point4d& operator+=( const Point4d& rhs )
        {
            xyzw[0] += rhs.xyzw[0];
            xyzw[1] += rhs.xyzw[1];
            xyzw[2] += rhs.xyzw[2];
            xyzw[3] += rhs.xyzw[3];
            return *this;
        }

        operator const float* () const
        {
            return xyzw;
        }

        operator float* ()
        {
            return xyzw;
        }

        point4_t xyzw = {};
    };

    inline Point2d::Point2d( const float* unaliased f )
    {
        xy[0] = f[0];
        xy[1] = f[1];
    }

    inline Point3d::Point3d( const float* unaliased f )
    {
        xyz[0] = f[0];
        xyz[1] = f[1];
        xyz[2] = f[2];
    }

    inline Point4d::Point4d( const float* unaliased f )
    {
        xyzw[0] = f[0];
        xyzw[1] = f[1];
        xyzw[2] = f[2];
        xyzw[3] = f[3];
    }

    inline Point2d::Point2d( float x, float y )
    {
        xy[0] = x;
        xy[1] = y;
    }

    inline Point3d::Point3d( float x, float y, float z )
    {
        xyz[0] = x;
        xyz[1] = y;
        xyz[2] = z;
    }

    inline Point4d::Point4d( float x, float y, float z, float w )
    {
        xyzw[0] = x;
        xyzw[1] = y;
        xyzw[2] = z;
        xyzw[3] = w;
    }

    inline bool equal( const float lhs, const float rhs)
    {
      return std::equal_to<float>()(lhs,rhs);
    }


    /// Lexicographical less than.
    inline bool lt2( const float* unaliased lhs, const float* unaliased rhs )
    {
        return ( ( lhs[0] < rhs[0] ) ) || ( equal(lhs[0],rhs[0]) && ( lhs[1] < rhs[1] ) );
    }

    /// Lexicographical less than.
    inline bool lt3( const float* unaliased lhs, const float* unaliased rhs )
    {
        return ( ( lhs[0] < rhs[0] ) ) || ( ( equal(lhs[0],rhs[0]) ) && ( lhs[1] < rhs[1] ) )
               || ( ( equal(lhs[0],rhs[0]) ) && ( equal(lhs[1],rhs[1]) ) && ( lhs[2] < rhs[2] ) );
    }

    /// Lexicographical less than.
    inline bool lt4( const float* unaliased lhs, const float* unaliased rhs )
    {
        return ( ( lhs[0] < rhs[0] ) ) || ( ( equal(lhs[0],rhs[0]) ) && ( lhs[1] < rhs[1] ) )
               || ( ( equal(lhs[0],rhs[0]) ) && ( equal(lhs[1],rhs[1]) ) && ( lhs[2] < rhs[2] ) )
               || ( ( equal(lhs[0],rhs[0]) ) && ( equal(lhs[1],rhs[1]) ) && ( equal(lhs[2],rhs[2]) ) && ( lhs[3] < rhs[3] ) );
    }

    /// Lexicographical greater than.
    inline bool gt2( const float* unaliased lhs, const float* unaliased rhs )
    {
        // If A < B => B > A
        return lt2( rhs, lhs );
    }

    /// Lexicographical greater than.
    inline bool gt3( const float* unaliased lhs, const float* unaliased rhs )
    {
        // If A < B => B > A
        return lt3( rhs, lhs );
    }

    /// Lexicographical less than.
    inline bool gt4( const float* unaliased lhs, const float* unaliased rhs )
    {
        // If A < B => B > A
        return lt4( rhs, lhs );
    }

    inline bool same2( const float* unaliased lhs, const float* unaliased rhs )
    {
        return equal(lhs[0],rhs[0]) && equal(lhs[1],rhs[1]);
    }

    inline bool same3( const float* unaliased lhs, const float* unaliased rhs )
    {
        return equal(lhs[0],rhs[0]) && equal(lhs[1],rhs[1]) && equal(lhs[2],rhs[2]);
    }

    inline bool same4( const float* unaliased lhs, const float* unaliased rhs )
    {
        return equal(lhs[0],rhs[0]) && equal(lhs[1],rhs[1]) && equal(lhs[2],rhs[2]) && equal(lhs[3],rhs[3]);
    }

    inline bool operator==( const Point2d& lhs, const Point2d& rhs )
    {
        return same2( lhs.xy, rhs.xy );
    }

    inline bool operator==( const Point3d& lhs, const Point3d& rhs )
    {
        return same3( lhs.xyz, rhs.xyz );
    }

    inline bool operator==( const Point4d& lhs, const Point4d& rhs )
    {
        return same4( lhs.xyzw, rhs.xyzw );
    }

    inline bool operator!=( const Point2d& lhs, const Point2d& rhs )
    {
        return false == same2( lhs.xy, rhs.xy );
    }

    inline bool operator!=( const Point3d& lhs, const Point3d& rhs )
    {
        return false == same3( lhs.xyz, rhs.xyz );
    }

    inline bool operator!=( const Point4d& lhs, const Point4d& rhs )
    {
        return false == same4( lhs.xyzw, rhs.xyzw );
    }

    inline bool operator<=( const Point2d& lhs, const Point2d& rhs )
    {
        return lt2( lhs.xy, rhs.xy ) || same2( lhs.xy, rhs.xy );
    }

    inline bool operator<=( const Point3d& lhs, const Point3d& rhs )
    {
        return lt3( lhs.xyz, rhs.xyz ) || same3( lhs.xyz, rhs.xyz );
    }

    inline bool operator<=( const Point4d& lhs, const Point4d& rhs )
    {
        return lt4( lhs.xyzw, rhs.xyzw ) || same4( lhs.xyzw, rhs.xyzw );
    }

    inline bool operator>=( const Point2d& lhs, const Point2d& rhs )
    {
        return gt2( lhs.xy, rhs.xy ) || same2( lhs.xy, rhs.xy );
    }

    inline bool operator>=( const Point3d& lhs, const Point3d& rhs )
    {
        return gt3( lhs.xyz, rhs.xyz ) || same3( lhs.xyz, rhs.xyz );
    }

    inline bool operator>=( const Point4d& lhs, const Point4d& rhs )
    {
        return gt4( lhs.xyzw, rhs.xyzw ) || same4( lhs.xyzw, rhs.xyzw );
    }

    inline bool operator<( const Point2d& lhs, const Point2d& rhs )
    {
        return lt2( lhs.xy, rhs.xy );
    }

    inline bool operator<( const Point3d& lhs, const Point3d& rhs )
    {
        return lt3( lhs.xyz, rhs.xyz );
    }

    inline bool operator<( const Point4d& lhs, const Point4d& rhs )
    {
        return lt4( lhs.xyzw, rhs.xyzw );
    }

    inline bool operator>( const Point2d& lhs, const Point2d& rhs )
    {
        return gt2( lhs.xy, rhs.xy );
    }

    inline bool operator>( const Point3d& lhs, const Point3d& rhs )
    {
        return gt3( lhs.xyz, rhs.xyz );
    }

    inline bool operator>( const Point4d& lhs, const Point4d& rhs )
    {
        return gt4( lhs.xyzw, rhs.xyzw );
    }

    inline void copy2( const float* unaliased lhs, float* unaliased rhs )
    {
        ASSERT_UNALIASED( lhs, rhs, 2, 2 );
        rhs[0] = lhs[0];
        rhs[1] = lhs[1];
    }

    inline void copy3( const float* unaliased lhs, float* unaliased rhs )
    {
        ASSERT_UNALIASED( lhs, rhs, 3, 3 );
        rhs[0] = lhs[0];
        rhs[1] = lhs[1];
        rhs[2] = lhs[2];
    }

    inline void copy4( const float* unaliased lhs, float* unaliased rhs )
    {
        ASSERT_UNALIASED( lhs, rhs, 4, 4 );
        rhs[0] = lhs[0];
        rhs[1] = lhs[1];
        rhs[2] = lhs[2];
        rhs[3] = lhs[3];
    }

    inline float sq( float f )
    {
        return f * f;
    }

    inline float distance_squared2( const float* unaliased lhs, const float* unaliased rhs )
    {
        return ( ( lhs[0] - rhs[0] ) * ( lhs[0] - rhs[0] ) ) + ( ( lhs[1] - rhs[1] ) * ( lhs[1] - rhs[1] ) );
    }

    inline float distance2( const float* unaliased lhs, const float* unaliased rhs )
    {
        return std::sqrt( distance_squared2( lhs, rhs ) );
    }

    inline float length2(  float const * const unaliased p )
    {
        return std::sqrt( ( p[0] * p[0] ) + ( p[1] * p[1] ) );
    }

    inline float length3( float const * const unaliased p )
    {
        return std::sqrt( ( p[0] * p[0] ) + ( p[1] * p[1] ) + ( p[2] * p[2] ) );
    }

    /// Returns 0 if a is nearer to p than a, else returns 1.
    inline unsigned int nearest2( float const * const unaliased p,
                                  float const * const unaliased a,
                                  float const * const unaliased b )
    {
        assert( a != p );
        assert( b != p );
        return distance2( p, a ) < distance2( p, b ) ? 0u : 1u;
    }

    /// Returns 0 if a is further from p than a, else returns 1.
    inline unsigned int furthest2( float const * const unaliased p,
                                   float const * const unaliased a,
                                   float const * const unaliased b )
    {
        assert( a != p );
        assert( b != p );
        return distance2( p, a ) > distance2( p, b ) ? 0u : 1u;
    }

    inline float distance3( float const * const unaliased lhs, float const * const unaliased rhs )
    {
        return std::sqrt( ( ( lhs[0] - rhs[0] ) * ( lhs[0] - rhs[0] ) )
                          + ( ( lhs[1] - rhs[1] ) * ( lhs[1] - rhs[1] ) )
                          + ( ( lhs[2] - rhs[2] ) * ( lhs[2] - rhs[2] ) ) );
    }

    inline float distance_squared3( const float* unaliased lhs, const float* unaliased rhs )
    {
        return ( ( lhs[0] - rhs[0] ) * ( lhs[0] - rhs[0] ) ) + ( ( lhs[1] - rhs[1] ) * ( lhs[1] - rhs[1] ) )
               + ( ( lhs[2] - rhs[2] ) * ( lhs[2] - rhs[2] ) );
    }

    inline float length_squared3( const float* unaliased p )
    {
        return p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
    }

    /// result = op0 - op1
    inline void sub2( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 2, 2 );
        ASSERT_UNALIASED( op1, result, 2, 2 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[0] - op1[0];
        result[1] = op0[1] - op1[1];
    }

    /// result = op0 + op1
    inline void add2( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 2, 2 );
        ASSERT_UNALIASED( op1, result, 2, 2 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[0] + op1[0];
        result[1] = op0[1] + op1[1];
    }

    /// result = op0 + op1
    inline void add3( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 3, 3 );
        ASSERT_UNALIASED( op1, result, 3, 3 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[0] + op1[0];
        result[1] = op0[1] + op1[1];
        result[2] = op0[2] + op1[2];
    }

    /// result = op0 + op1
    inline void add4( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 4, 4 );
        ASSERT_UNALIASED( op1, result, 4, 4 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[0] + op1[0];
        result[1] = op0[1] + op1[1];
        result[2] = op0[2] + op1[2];
        result[3] = op0[3] + op1[3];
    }

    /// result = op0 - op1
    inline void sub3( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 3, 3 );
        ASSERT_UNALIASED( op1, result, 3, 3 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[0] - op1[0];
        result[1] = op0[1] - op1[1];
        result[2] = op0[2] - op1[2];
    }

    inline float dot3( const float* unaliased lhs, const float* unaliased rhs )
    {
        assert( lhs != rhs );
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
    }

    inline void cross3( const float* unaliased op0, const float* unaliased op1, float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 3, 3 );
        ASSERT_UNALIASED( op1, result, 3, 3 );

        assert( op0 != op1 );
        assert( op0 != result );

        result[0] = op0[1] * op1[2] - op0[2] * op1[1];
        result[1] = op0[2] * op1[0] - op0[0] * op1[2]; // NOTE -( x * op1[2] - z * op1[0] )
        result[2] = op0[0] * op1[1] - op0[1] * op1[0];
    }

    inline float normalise3( float* unaliased ans )
    {
        const float f = std::sqrt( ( ans[0] * ans[0] ) + ( ans[1] * ans[1] ) + ( ans[2] * ans[2] ) );
        assert( f > 0.0001f );

        ans[0] /= f;
        ans[1] /= f;
        ans[2] /= f;

        return f;
    }

    inline float normalise2( float* unaliased ans )
    {
        const float f = std::sqrt( ( ans[0] * ans[0] ) + ( ans[1] * ans[1] ) );

        ans[0] /= f;
        ans[1] /= f;

        return f;
    }

    inline void midpoint3( const float* unaliased op0,
                           const float* unaliased op1,
                           float* unaliased result )
    {
        ASSERT_UNALIASED( op0, result, 3, 3 );
        ASSERT_UNALIASED( op1, result, 3, 3 );

        result[0] = ( 0.5f * ( op1[0] - op0[0] ) ) + op0[0];
        result[1] = ( 0.5f * ( op1[1] - op0[1] ) ) + op0[1];
        result[2] = ( 0.5f * ( op1[2] - op0[2] ) ) + op0[2];
    }

    inline void mul2( float* unaliased v3, float f )
    {
        v3[0] *= f;
        v3[1] *= f;
    }

    inline void mul3( float* unaliased v3, float f )
    {
        v3[0] *= f;
        v3[1] *= f;
        v3[2] *= f;
    }
    inline void mul4( float* unaliased v4, float f )
    {
        v4[0] *= f;
        v4[1] *= f;
        v4[2] *= f;
        v4[3] *= f;
    }

    /// D = C + A * B
    inline void madd2( const float* unaliased A,
                       const float* unaliased B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B[0];
        D[1] = C[1] + A[1] * B[1];
    }

    /// D = C + A * B
    inline void madd2( const float* unaliased A,
                       const float B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B;
        D[1] = C[1] + A[1] * B;
    }

    /// D = C + A * B
    inline void madd3( const float* unaliased A,
                       const float* unaliased B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B[0];
        D[1] = C[1] + A[1] * B[1];
        D[2] = C[2] + A[2] * B[2];
    }

    /// D = C + A * B
    inline void madd3( const float* unaliased A,
                       const float B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B;
        D[1] = C[1] + A[1] * B;
        D[2] = C[2] + A[2] * B;
    }

    /// D = C + A * B
    inline void madd4( const float* unaliased A,
                       const float* unaliased B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B[0];
        D[1] = C[1] + A[1] * B[1];
        D[2] = C[2] + A[2] * B[2];
        D[3] = C[3] + A[3] * B[3];
    }

    /// D = C + A * B
    inline void madd4( const float* unaliased A,
                       const float B,
                       const float* unaliased C,
                       float* unaliased D )
    {
        D[0] = C[0] + A[0] * B;
        D[1] = C[1] + A[1] * B;
        D[2] = C[2] + A[2] * B;
        D[3] = C[3] + A[3] * B;
    }

    inline void div3( float* unaliased v3, float f )
    {
        v3[0] /= f;
        v3[1] /= f;
        v3[2] /= f;
    }

    inline void div4( float* unaliased v4, float f )
    {
        v4[0] /= f;
        v4[1] /= f;
        v4[2] /= f;
        v4[3] /= f;
    }

    inline void homogenise( float* unaliased v4 )
    {
        assert( std::abs( v4[3] ) > 0.001f );
        v4[0] /= v4[3];
        v4[1] /= v4[3];
        v4[2] /= v4[3];
        v4[3] = 1.0f;
    }

    */
}

#endif
