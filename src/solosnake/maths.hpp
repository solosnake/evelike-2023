#ifndef SOLOSNAKE_MATHS_HPP
#define SOLOSNAKE_MATHS_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <limits>

/// Uncomment (or fix) if your compiler does
/// not support restrict/__restrict or __restrict__.
// #define __restrict

namespace solosnake
{
    constexpr auto Pi{3.14159265358979324f};

    struct Point3d {
        float x, y, z;

        /// Permits Point3d to be passed as const float*
        operator const float* () const noexcept
        {
            return &x;
        }
    };

    struct Point4d {
        float x, y, z, w;

        /// Permits Matrix4x4 to be passed as const float*
        operator const float* () const noexcept
        {
            return &x;
        }
    };

    struct Sphere {
        Point3d centre;
        float   radius;
    };

    inline bool operator == (const Point3d& lhs, const Point3d& rhs) noexcept
    {
        return lhs.x == rhs.x and lhs.y == rhs.y and lhs.z == rhs.z;
    }

    inline bool operator == (const Sphere& lhs, const Sphere& rhs) noexcept
    {
        return lhs.centre == rhs.centre and lhs.radius == rhs.radius;
    }

    inline Point3d operator - (const Point3d& p) noexcept
    {
        return Point3d{ -p.x, -p.y, -p.z };
    }

    inline Point3d to_point(const std::array<float,3u>& a) noexcept
    {
        return Point3d{ a[0], a[1], a[2] };
    }

    /// Column-major order (OpenGL friendly).
    ///
    /// For programming purposes, OpenGL matrices are 16-value arrays with base
    /// vectors laid out contiguously in memory. The translation components
    /// occupy the 13th, 14th, and 15th elements of the 16-element matrix, where
    /// indices are numbered from 1 to 16 as described in section 2.11.2 of the
    /// OpenGL 2.1 Specification.
    ///
    /// Note that this is the following matrix when doing maths:
    /// | m11 m12 m13 m14 |
    /// | m21 m22 m23 m24 |
    /// | m31 m32 m33 m34 |
    /// | m41 m42 m43 m44 |
    struct Matrix4x4 {
        float m11, m21, m31, m41, //  0  1  2  3
              m12, m22, m32, m42, //  4  5  6  7
              m13, m23, m33, m43, //  8  9 10 11
              m14, m24, m34, m44; // 12 13 14 15

        /// Permits Matrix4x4 to be passed as const float*
        operator const float* () const noexcept
        {
            return &m11;
        }

        /// Permits Matrix4x4 to be passed as float*
        operator float* () noexcept
        {
            return &m11;
        }
    };

    static_assert( sizeof(Point3d) == (sizeof(float) * 3),
                   "Point3d is wrong size.");

    static_assert( sizeof(Matrix4x4) == (sizeof(float) * 16),
                   "Matrix4x4 is wrong size.");

    static_assert( sizeof(Matrix4x4) == (sizeof(std::array<float,16>)),
                   "std::array<float,16> is wrong size.");

    constexpr inline Matrix4x4 Identity4x4() noexcept
    {
        return {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    inline Point3d operator + (const Point3d& p0, const Point3d& p1) noexcept
    {
        return Point3d{ p0.x + p1.x, p0.y + p1.y, p0.z + p1.z };
    }

    inline float distance(const Point3d& v0, const Point3d& v1) noexcept
    {
        return std::sqrt( (v0.x - v1.x) * (v0.x - v1.x) +
                          (v0.y - v1.y) * (v0.y - v1.y) +
                          (v0.z - v1.z) * (v0.z - v1.z) );
    }

    inline Matrix4x4 scale_4x4(const float x,
                               const float y,
                               const float z) noexcept
    {
        return Matrix4x4 {
            x,    0.0f, 0.0f, 0.0f,
            0.0f,    y, 0.0f, 0.0f,
            0.0f, 0.0f,    z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    inline Matrix4x4 translation_4x4(const float x,
                                     const float y,
                                     const float z) noexcept
    {
        return Matrix4x4 {
            1.0f, 0.0f, 0.0f,    x,
            0.0f, 1.0f, 0.0f,    y,
            0.0f, 0.0f, 1.0f,    z,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    /// Create a rotation about the x axis matrix.
    /// @param a Angle in radians.
    inline Matrix4x4 rotation_4x4_x(const float a) noexcept
    {
        return Matrix4x4 {
            1.0f,        0.0f,        0.0f, 0.0f,
            0.0f, std::cos(a), -std::sin(a), 0.0f,
            0.0f, std::sin(a),  std::cos(a), 0.0f,
            0.0f,        0.0f,        0.0f, 1.0f,
        };
    }

    /// Create a rotation about the y axis matrix.
    /// @param a Angle in radians.
    inline Matrix4x4 rotation_4x4_y(const float a) noexcept
    {
        return Matrix4x4 {
            std::cos(a), 0.0f,  std::sin(a), 0.0f,
                   0.0f, 1.0f,         0.0f, 0.0f,
           -std::sin(a), 0.0f,  std::cos(a), 0.0f,
                   0.0f, 0.0f,         0.0f, 1.0f,
        };
    }

    /// Create a rotation about the z axis matrix.
    /// @param a Angle in radians.
    inline Matrix4x4 rotation_4x4_z(const float a) noexcept
    {
        return Matrix4x4 {
             std::cos(a), -std::sin(a), 0.0f, 0.0f,
             std::sin(a),  std::cos(a), 0.0f, 0.0f,
                    0.0f,         0.0f, 1.0f, 0.0f,
                    0.0f,         0.0f, 0.0f, 1.0f,
        };
    }

    /// dst4x4 (4x4) = lhs (4x4) x rhs (4x4)
    /// In terms of perceived effect, rhs happens after lhs.
    inline void mul_4x4( const float* __restrict lhs,
                         const float* __restrict rhs,
                         float* __restrict dst4x4 ) noexcept
    {
        assert( lhs != rhs );
        assert( lhs != dst4x4 );

        dst4x4[0] = lhs[0] * rhs[0] + lhs[4] * rhs[1] + lhs[8]  * rhs[2] + lhs[12] * rhs[3];
        dst4x4[1] = lhs[1] * rhs[0] + lhs[5] * rhs[1] + lhs[9]  * rhs[2] + lhs[13] * rhs[3];
        dst4x4[2] = lhs[2] * rhs[0] + lhs[6] * rhs[1] + lhs[10] * rhs[2] + lhs[14] * rhs[3];
        dst4x4[3] = lhs[3] * rhs[0] + lhs[7] * rhs[1] + lhs[11] * rhs[2] + lhs[15] * rhs[3];

        dst4x4[4] = lhs[0] * rhs[4] + lhs[4] * rhs[5] + lhs[8]  * rhs[6] + lhs[12] * rhs[7];
        dst4x4[5] = lhs[1] * rhs[4] + lhs[5] * rhs[5] + lhs[9]  * rhs[6] + lhs[13] * rhs[7];
        dst4x4[6] = lhs[2] * rhs[4] + lhs[6] * rhs[5] + lhs[10] * rhs[6] + lhs[14] * rhs[7];
        dst4x4[7] = lhs[3] * rhs[4] + lhs[7] * rhs[5] + lhs[11] * rhs[6] + lhs[15] * rhs[7];

        dst4x4[8] = lhs[0]  * rhs[8] + lhs[4] * rhs[9] + lhs[8]  * rhs[10] + lhs[12] * rhs[11];
        dst4x4[9] = lhs[1]  * rhs[8] + lhs[5] * rhs[9] + lhs[9]  * rhs[10] + lhs[13] * rhs[11];
        dst4x4[10] = lhs[2] * rhs[8] + lhs[6] * rhs[9] + lhs[10] * rhs[10] + lhs[14] * rhs[11];
        dst4x4[11] = lhs[3] * rhs[8] + lhs[7] * rhs[9] + lhs[11] * rhs[10] + lhs[15] * rhs[11];

        dst4x4[12] = lhs[0] * rhs[12] + lhs[4] * rhs[13] + lhs[8]  * rhs[14] + lhs[12] * rhs[15];
        dst4x4[13] = lhs[1] * rhs[12] + lhs[5] * rhs[13] + lhs[9]  * rhs[14] + lhs[13] * rhs[15];
        dst4x4[14] = lhs[2] * rhs[12] + lhs[6] * rhs[13] + lhs[10] * rhs[14] + lhs[14] * rhs[15];
        dst4x4[15] = lhs[3] * rhs[12] + lhs[7] * rhs[13] + lhs[11] * rhs[14] + lhs[15] * rhs[15];
    }

    /// dst4x4 (4x4) = lhs (4x4) x rhs (4x4)
    /// In terms of perceived effect, rhs happens after lhs.
    inline void mul_4x4( const Matrix4x4* __restrict lhs,
                         const Matrix4x4* __restrict rhs,
                         Matrix4x4* __restrict dst4x4 ) noexcept
    {
        assert( lhs != rhs );
        assert( lhs != dst4x4 );
        dst4x4->m11 = lhs->m11 * rhs->m11 + lhs->m12 * rhs->m21 + lhs->m13 * rhs->m31 + lhs->m14 * rhs->m41;
        dst4x4->m21 = lhs->m21 * rhs->m11 + lhs->m22 * rhs->m21 + lhs->m23 * rhs->m31 + lhs->m24 * rhs->m41;
        dst4x4->m31 = lhs->m31 * rhs->m11 + lhs->m32 * rhs->m21 + lhs->m33 * rhs->m31 + lhs->m34 * rhs->m41;
        dst4x4->m41 = lhs->m41 * rhs->m11 + lhs->m42 * rhs->m21 + lhs->m43 * rhs->m31 + lhs->m44 * rhs->m41;

        dst4x4->m12 = lhs->m11 * rhs->m12 + lhs->m12 * rhs->m22 + lhs->m13 * rhs->m32 + lhs->m14 * rhs->m42;
        dst4x4->m22 = lhs->m21 * rhs->m12 + lhs->m22 * rhs->m22 + lhs->m23 * rhs->m32 + lhs->m24 * rhs->m42;
        dst4x4->m32 = lhs->m31 * rhs->m12 + lhs->m32 * rhs->m22 + lhs->m33 * rhs->m32 + lhs->m34 * rhs->m42;
        dst4x4->m42 = lhs->m41 * rhs->m12 + lhs->m42 * rhs->m22 + lhs->m43 * rhs->m32 + lhs->m44 * rhs->m42;

        dst4x4->m13 = lhs->m11 * rhs->m13 + lhs->m12 * rhs->m23 + lhs->m13 * rhs->m33 + lhs->m14 * rhs->m43;
        dst4x4->m23 = lhs->m21 * rhs->m13 + lhs->m22 * rhs->m23 + lhs->m23 * rhs->m33 + lhs->m24 * rhs->m43;
        dst4x4->m33 = lhs->m31 * rhs->m13 + lhs->m32 * rhs->m23 + lhs->m33 * rhs->m33 + lhs->m34 * rhs->m43;
        dst4x4->m43 = lhs->m41 * rhs->m13 + lhs->m42 * rhs->m23 + lhs->m43 * rhs->m33 + lhs->m44 * rhs->m43;

        dst4x4->m14 = lhs->m11 * rhs->m14 + lhs->m12 * rhs->m24 + lhs->m13 * rhs->m34 + lhs->m14 * rhs->m44;
        dst4x4->m24 = lhs->m21 * rhs->m14 + lhs->m22 * rhs->m24 + lhs->m23 * rhs->m34 + lhs->m24 * rhs->m44;
        dst4x4->m34 = lhs->m31 * rhs->m14 + lhs->m32 * rhs->m24 + lhs->m33 * rhs->m34 + lhs->m34 * rhs->m44;
        dst4x4->m44 = lhs->m41 * rhs->m14 + lhs->m42 * rhs->m24 + lhs->m43 * rhs->m34 + lhs->m44 * rhs->m44;
    }

    /// Returns (4x4) x (p4)
    inline Point4d mul_4x1( const Matrix4x4* __restrict rhs,
                            const Point4d* __restrict p ) noexcept
    {
        return Point4d{
            p->x * rhs->m11 + p->y * rhs->m21 + p->z * rhs->m31 + p->w * rhs->m41,
            p->x * rhs->m12 + p->y * rhs->m22 + p->z * rhs->m32 + p->w * rhs->m42,
            p->x * rhs->m13 + p->y * rhs->m23 + p->z * rhs->m33 + p->w * rhs->m43,
            p->x * rhs->m14 + p->y * rhs->m24 + p->z * rhs->m34 + p->w * rhs->m44
        };
    }

    /// Returns m (4x4) x p (4x1)
    inline Point4d mul_4x1( const float* __restrict m,
                            const Point4d* __restrict p ) noexcept
    {
        return Point4d{
            p->x * m[0]  + p->y * m[1]  + p->z * m[2]  + p->w * m[3],
            p->x * m[4]  + p->y * m[5]  + p->z * m[6]  + p->w * m[7],
            p->x * m[8]  + p->y * m[9]  + p->z * m[10] + p->w * m[11],
            p->x * m[12] + p->y * m[13] + p->z * m[14] + p->w * m[15]
        };
    }

    /// Returns lhs (4x4) x rhs (4x4)
    /// In terms of perceived effect, rhs happens after lhs.
    inline Matrix4x4 mul_4x4( const Matrix4x4* __restrict lhs,
                              const Matrix4x4* __restrict rhs) noexcept
    {
        assert( lhs != rhs );
        return Matrix4x4 {
            lhs->m11 * rhs->m11 + lhs->m12 * rhs->m21 + lhs->m13 * rhs->m31 + lhs->m14 * rhs->m41,
            lhs->m21 * rhs->m11 + lhs->m22 * rhs->m21 + lhs->m23 * rhs->m31 + lhs->m24 * rhs->m41,
            lhs->m31 * rhs->m11 + lhs->m32 * rhs->m21 + lhs->m33 * rhs->m31 + lhs->m34 * rhs->m41,
            lhs->m41 * rhs->m11 + lhs->m42 * rhs->m21 + lhs->m43 * rhs->m31 + lhs->m44 * rhs->m41,

            lhs->m11 * rhs->m12 + lhs->m12 * rhs->m22 + lhs->m13 * rhs->m32 + lhs->m14 * rhs->m42,
            lhs->m21 * rhs->m12 + lhs->m22 * rhs->m22 + lhs->m23 * rhs->m32 + lhs->m24 * rhs->m42,
            lhs->m31 * rhs->m12 + lhs->m32 * rhs->m22 + lhs->m33 * rhs->m32 + lhs->m34 * rhs->m42,
            lhs->m41 * rhs->m12 + lhs->m42 * rhs->m22 + lhs->m43 * rhs->m32 + lhs->m44 * rhs->m42,

            lhs->m11 * rhs->m13 + lhs->m12 * rhs->m23 + lhs->m13 * rhs->m33 + lhs->m14 * rhs->m43,
            lhs->m21 * rhs->m13 + lhs->m22 * rhs->m23 + lhs->m23 * rhs->m33 + lhs->m24 * rhs->m43,
            lhs->m31 * rhs->m13 + lhs->m32 * rhs->m23 + lhs->m33 * rhs->m33 + lhs->m34 * rhs->m43,
            lhs->m41 * rhs->m13 + lhs->m42 * rhs->m23 + lhs->m43 * rhs->m33 + lhs->m44 * rhs->m43,

            lhs->m11 * rhs->m14 + lhs->m12 * rhs->m24 + lhs->m13 * rhs->m34 + lhs->m14 * rhs->m44,
            lhs->m21 * rhs->m14 + lhs->m22 * rhs->m24 + lhs->m23 * rhs->m34 + lhs->m24 * rhs->m44,
            lhs->m31 * rhs->m14 + lhs->m32 * rhs->m24 + lhs->m33 * rhs->m34 + lhs->m34 * rhs->m44,
            lhs->m41 * rhs->m14 + lhs->m42 * rhs->m24 + lhs->m43 * rhs->m34 + lhs->m44 * rhs->m44,
        };
    }

    /// fov_y:   Specifies the total field of view angle, in RADIANS, in the y direction.
    /// aspect:  Specifies the aspect ratio that determines the field of view in the x direction.
    ///          The aspect ratio is the ratio of x (width) to y (height) = (W/H)
    /// zNear:   Specifies the distance from the viewer to the near clipping plane (always positive).
    /// zFar:    Specifies the distance from the viewer to the far clipping plane (always positive).
    inline void load_perspective( const float fov_y,
                                  const float aspect,
                                  const float zNear,
                                  const float zFar,
                                  Matrix4x4& dst4x4 ) noexcept
    {
        assert( zNear > 0.0f );
        assert( zFar > 0.0f );
        assert( std::abs( aspect ) > std::numeric_limits<float>::epsilon() );
        assert( fov_y > std::numeric_limits<float>::epsilon() );

        dst4x4.m22 = 1.0f / std::tan( fov_y * 0.5f );

        dst4x4.m11 = dst4x4.m22 / aspect;
        dst4x4.m21 = 0.0f;
        dst4x4.m31 = 0.0f;
        dst4x4.m41 = 0.0f;

        dst4x4.m12 = 0.0f;
        dst4x4.m32 = 0.0f;
        dst4x4.m42 = 0.0f;

        dst4x4.m13 = 0.0f;
        dst4x4.m23 = 0.0f;
        dst4x4.m33 = ( zFar + zNear ) / ( zNear - zFar );
        dst4x4.m43 = -1.0f;

        dst4x4.m14 = 0.0f;
        dst4x4.m24 = 0.0f;
        dst4x4.m34 = ( 2.0f * zFar * zNear ) / ( zNear - zFar );
        dst4x4.m44 = 0.0f;
    }

    void load_look_at( const Point3d* __restrict lookedAt3f,
                       const Point3d* __restrict position3f,
                       const Point3d* __restrict upVec3f,
                       Matrix4x4* __restrict result ) noexcept;

/*
    inline float x_4x4( const float* m4x4 ) noexcept
    {
        return m4x4[12];
    }

    inline float y_4x4( const float* m4x4 ) noexcept
    {
        return m4x4[13];
    }

    inline float z_4x4( const float* m4x4 ) noexcept
    {
        return m4x4[14];
    }

    inline Point3d up_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ m4x4[4], m4x4[5], m4x4[6] };
    }

    inline Point3d down_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ -m4x4[4], -m4x4[5], -m4x4[6] };
    }

    inline Point3d left_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ m4x4[0], m4x4[1], m4x4[2] };
    }

    inline Point3d right_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ -m4x4[0], -m4x4[1], -m4x4[2] };
    }

    inline Point3d forward_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ m4x4[8], m4x4[9], m4x4[10] };
    }

    inline Point3d backward_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ -m4x4[8], -m4x4[9], -m4x4[10] };
    }

    inline Point3d location_4x4( const float* m4x4 ) noexcept
    {
        return Point3d{ m4x4[12], m4x4[13], m4x4[14] };
    }

    inline void load_translation_4x4( float x, float y, float z, float* dst4x4 ) noexcept
    {
        dst4x4[0] = 1.0f;
        dst4x4[1] = 0.0f;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;
        dst4x4[4] = 0.0f;
        dst4x4[5] = 1.0f;
        dst4x4[6] = 0.0f;
        dst4x4[7] = 0.0f;
        dst4x4[8] = 0.0f;
        dst4x4[9] = 0.0f;
        dst4x4[10] = 1.0f;
        dst4x4[11] = 0.0f;
        dst4x4[12] = x;
        dst4x4[13] = y;
        dst4x4[14] = z;
        dst4x4[15] = 1.0f;
    }

    inline void load_translation_4x4( const float* point3, float* dst4x4 ) noexcept
    {
        dst4x4[0] = 1.0f;
        dst4x4[1] = 0.0f;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;
        dst4x4[4] = 0.0f;
        dst4x4[5] = 1.0f;
        dst4x4[6] = 0.0f;
        dst4x4[7] = 0.0f;
        dst4x4[8] = 0.0f;
        dst4x4[9] = 0.0f;
        dst4x4[10] = 1.0f;
        dst4x4[11] = 0.0f;
        dst4x4[12] = point3[0];
        dst4x4[13] = point3[1];
        dst4x4[14] = point3[2];
        dst4x4[15] = 1.0f;
    }

    inline void set_translation_4x4( const float* point3, float* dst4x4 ) noexcept
    {
        dst4x4[12] = point3[0];
        dst4x4[13] = point3[1];
        dst4x4[14] = point3[2];
    }

    inline void set_translation_4x4( float x, float y, float z, float* dst4x4 ) noexcept
    {
        dst4x4[12] = x;
        dst4x4[13] = y;
        dst4x4[14] = z;
    }

    /// For programming purposes, OpenGL matrices are 16-value arrays with base
    /// vectors laid out contiguously in memory. The translation components occupy
    /// the 13th, 14th, and 15th elements of the 16-element matrix, where indices
    /// are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1
    /// Specification.
    inline void transpose_4x4( const float* __restrict src,
                               float* __restrict dst4x4 ) noexcept
    {
        dst4x4[0] = src[0];
        dst4x4[1] = src[4];
        dst4x4[2] = src[8];
        dst4x4[3] = src[12];

        dst4x4[4] = src[1];
        dst4x4[5] = src[5];
        dst4x4[6] = src[9];
        dst4x4[7] = src[13];

        dst4x4[8] = src[2];
        dst4x4[9] = src[6];
        dst4x4[10] = src[10];
        dst4x4[11] = src[14];

        dst4x4[12] = src[3];
        dst4x4[13] = src[7];
        dst4x4[14] = src[11];
        dst4x4[15] = src[15];
    }

    /// dst4x1 (XYZW) = src (XYZW vector) * m (4x4 matrix)
    /// This would greatly benefit from the array being arranged in
    /// the other direction (SIMD).
    inline void transform_4x1( const float* __restrict src,
                               const float* __restrict m4x4,
                               float* __restrict dst4x1 ) noexcept
    {
        dst4x1[0] = src[0] * m4x4[0] + src[1] * m4x4[4] + src[2] * m4x4[8] + src[3] * m4x4[12];
        dst4x1[1] = src[0] * m4x4[1] + src[1] * m4x4[5] + src[2] * m4x4[9] + src[3] * m4x4[13];
        dst4x1[2] = src[0] * m4x4[2] + src[1] * m4x4[6] + src[2] * m4x4[10] + src[3] * m4x4[14];
        dst4x1[3] = src[0] * m4x4[3] + src[1] * m4x4[7] + src[2] * m4x4[11] + src[3] * m4x4[15];
    }

    inline void load_identity_4x4( float* dst4x4 ) noexcept
    {
        dst4x4[0] = 1.0f;
        dst4x4[1] = 0.0f;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;
        dst4x4[4] = 0.0f;
        dst4x4[5] = 1.0f;
        dst4x4[6] = 0.0f;
        dst4x4[7] = 0.0f;
        dst4x4[8] = 0.0f;
        dst4x4[9] = 0.0f;
        dst4x4[10] = 1.0f;
        dst4x4[11] = 0.0f;
        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }
*/

/*
    /// Typically, the matrix mode is GL_PROJECTION, and (left bottom -nearVal)
    /// and (right top -nearVal) specify the points on the near clipping plane
    /// that are mapped to the lower left and upper right corners of the window,
    /// respectively, assuming that the eye is located at (0, 0, 0).
    /// -farVal specifies the location of the far clipping plane.
    /// Both nearVal and farVal can be either positive or negative.
    /// http://msdn.microsoft.com/en-us/library/dd373965%28v=vs.85%29.aspx
    /// http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
    inline void load_ortho( float left,
                            float right,
                            float bottom,
                            float top,
                            float nearVal,
                            float farVal,
                            float* dst4x4 ) noexcept
    {
        assert( std::fabs( right - left )     > ( 100 * std::numeric_limits<float>::epsilon() ) );
        assert( std::fabs( top - bottom )     > ( 100 * std::numeric_limits<float>::epsilon() ) );
        assert( std::fabs( farVal - nearVal ) > ( 100 * std::numeric_limits<float>::epsilon() ) );

        dst4x4[0] = 2.0f / ( right - left );
        dst4x4[1] = 0.0f;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;

        dst4x4[4] = 0.0f;
        dst4x4[5] = 2.0f / ( top - bottom );
        dst4x4[6] = 0.0f;
        dst4x4[7] = 0.0f;

        dst4x4[8] = 0.0f;
        dst4x4[9] = 0.0f;
        dst4x4[10] = -2.0f / ( farVal - nearVal );
        dst4x4[11] = 0.0f;

        dst4x4[12] = -( right + left ) / ( right - left );
        dst4x4[13] = -( top + bottom ) / ( top - bottom );
        dst4x4[15] = 1.0f;
        dst4x4[14] = -( farVal + nearVal ) / ( farVal - nearVal );
    }
*/

/*
    /// Returns the FOV angle in the x axis based on lhs FOV in the Y and aspect
    /// ratio of lhs view.
    inline float fov_x( float fov_y, float aspect ) noexcept
    {
        return 2.0f * std::atan( aspect * std::tan( fov_y * 0.5f ) );
    }
*/


/*
    /// http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    /// @param t    Expressed in radians.
    inline void load_rotation_3x3_unit_axis_angle( const float t,
                                                   const float * const __restrict axis3,
                                                   float* __restrict dst3x3 ) noexcept
    {
        const float cos0   = std::cos(t);
        const float sin0   = std::sin(t);
        const float l_cos0 = 1.0f - cos0;

        dst3x3[0] = axis3[0] * axis3[0] * l_cos0 + cos0;
        dst3x3[1] = axis3[0] * axis3[1] * l_cos0 - axis3[2] * sin0;
        dst3x3[2] = axis3[0] * axis3[2] * l_cos0 + axis3[1] * sin0;

        dst3x3[3] = axis3[1] * axis3[0] * l_cos0 + axis3[2] * sin0;
        dst3x3[4] = axis3[1] * axis3[1] * l_cos0 + cos0;
        dst3x3[5] = axis3[1] * axis3[2] * l_cos0 - axis3[0] * sin0;

        dst3x3[6] = axis3[2] * axis3[0] * l_cos0 - axis3[1] * sin0;
        dst3x3[7] = axis3[2] * axis3[1] * l_cos0 + axis3[0] * sin0;
        dst3x3[8] = axis3[2] * axis3[2] * l_cos0 + cos0;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_x_3x3( const float t, float* dst3x3 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst3x3[0] = 1.0f;
        dst3x3[1] = 0.0f;
        dst3x3[2] = 0.0f;

        dst3x3[3] = 0.0f;
        dst3x3[4] = c;
        dst3x3[5] = -s;

        dst3x3[6] = 0.0f;
        dst3x3[7] = s;
        dst3x3[8] = c;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_x_4x4( const float t, float* dst4x4 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst4x4[0] = 1.0f;
        dst4x4[1] = 0.0f;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;

        dst4x4[4] = 0.0f;
        dst4x4[5] = c;
        dst4x4[6] = -s;
        dst4x4[7] = 0.0f;

        dst4x4[8] = 0.0f;
        dst4x4[9] = s;
        dst4x4[10] = c;
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_y_3x3( const float t, float* dst3x3 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst3x3[0] = c;
        dst3x3[1] = 0.0f;
        dst3x3[2] = s;

        dst3x3[3] = 0.0f;
        dst3x3[4] = 1.0;
        dst3x3[5] = 0.0;

        dst3x3[6] = -s;
        dst3x3[7] = 0.0;
        dst3x3[8] = c;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_y_4x4( const float t, float* dst4x4 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst4x4[0] = c;
        dst4x4[1] = 0.0f;
        dst4x4[2] = s;
        dst4x4[3] = 0.0f;

        dst4x4[4] = 0.0f;
        dst4x4[5] = 1.0;
        dst4x4[6] = 0.0;
        dst4x4[7] = 0.0f;

        dst4x4[8] = -s;
        dst4x4[9] = 0.0;
        dst4x4[10] = c;
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_z_3x3( const float t, float* dst3x3 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst3x3[0] = c;
        dst3x3[1] = -s;
        dst3x3[2] = 0.0f;

        dst3x3[3] = s;
        dst3x3[4] = c;
        dst3x3[5] = 0.0;

        dst3x3[6] = 0.0f;
        dst3x3[7] = 0.0;
        dst3x3[8] = 1.0f;
    }

    /// @param t    Expressed in radians.
    inline void load_rotation_z_4x4( const float t, float* dst4x4 ) noexcept
    {
        float c = std::cos(t);
        float s = std::sin(t);

        dst4x4[0] = c;
        dst4x4[1] = -s;
        dst4x4[2] = 0.0f;
        dst4x4[3] = 0.0f;

        dst4x4[4] = s;
        dst4x4[5] = c;
        dst4x4[6] = 0.0;
        dst4x4[7] = 0.0f;

        dst4x4[8] = 0.0f;
        dst4x4[9] = 0.0;
        dst4x4[10] = 1.0f;
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }

    /// @param x    Expressed in radians.
    /// @param y    Expressed in radians.
    /// @param z    Expressed in radians.
    inline void load_rotation_xyz_4x4( const float x,
                                       const float y,
                                       const float z,
                                       float* dst4x4 ) noexcept
    {
        float cx = std::cos(x);
        float cy = std::cos(y);
        float cz = std::cos(z);
        float sx = std::sin(x);
        float sy = std::sin(y);
        float sz = std::sin(z);

        dst4x4[0] = cy * cz;
        dst4x4[1] = ( sx * sy * cz ) - ( cx * sz );
        dst4x4[2] = ( sx * sz ) + ( cx * sy * cz );
        dst4x4[3] = 0.0f;

        dst4x4[4] = cy * sz;
        dst4x4[5] = ( cx * cz ) + ( sx * sy * sz );
        dst4x4[6] = ( cx * sy * sz ) - ( sx * cz );
        dst4x4[7] = 0.0f;

        dst4x4[8] = -sy;
        dst4x4[9] = sx * cy;
        dst4x4[10] = cx * cy;
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }

    /// @param x    Expressed in radians.
    /// @param y    Expressed in radians.
    inline void load_rotation_xy_4x4( const float x,
                                      const float y,
                                      float* dst4x4 ) noexcept
    {
        float cx = std::cos(x);
        float sx = std::sin(x);
        float cy = std::cos(y);
        float sy = std::sin(y);

        dst4x4[0] = cy;
        dst4x4[1] = sx * sy;
        dst4x4[2] = cx * sy;
        dst4x4[3] = 0.0f;

        dst4x4[4] = 0.0f;
        dst4x4[5] = cx;
        dst4x4[6] = -sx;
        dst4x4[7] = 0.0f;

        dst4x4[8] = -sy;
        dst4x4[9] = sx * cy;
        dst4x4[10] = cx * cy;
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0f;
    }

    /// @param x    Expressed in radians.
    /// @param y    Expressed in radians.
    inline void load_rotation_xy_3x3( const float x,
                                      const float y,
                                      float* dst3x3 ) noexcept
    {
        float cx = std::cos(x);
        float sx = std::sin(x);
        float cy = std::cos(y);
        float sy = std::sin(y);

        dst3x3[0] = cy;
        dst3x3[1] = sx * sy;
        dst3x3[2] = cx * sy;

        dst3x3[3] = 0.0f;
        dst3x3[4] = cx;
        dst3x3[5] = -sx;

        dst3x3[6] = -sy;
        dst3x3[7] = sx * cy;
        dst3x3[8] = cx * cy;
    }

    /// @param ry    Expressed in radians.
    inline void load_xyz_ry_4x4( const float x,
                                 const float y,
                                 const float z,
                                 const float ry,
                                 float* dst4x4 ) noexcept
    {
        load_rotation_y_4x4( ry, dst4x4 );
        set_translation_4x4( x, y, z, dst4x4 );
    }

    inline bool identical_4x4( const float* lhs, const float* rhs ) noexcept
    {
        return lhs[0]  == rhs[0]  && lhs[1]  == rhs[1]  && lhs[2]  == rhs[2]  && lhs[3]  == rhs[3]
            && lhs[4]  == rhs[4]  && lhs[5]  == rhs[5]  && lhs[6]  == rhs[6]  && lhs[7]  == rhs[7]
            && lhs[8]  == rhs[8]  && lhs[9]  == rhs[9]  && lhs[10] == rhs[10] && lhs[11] == rhs[11]
            && lhs[12] == rhs[12] && lhs[13] == rhs[13] && lhs[14] == rhs[14] && lhs[15] == rhs[15];
    }

    float rotation_determinant_4x4( const float* m4x4 ) noexcept;

    bool invert_4x4( const float* __restrict src,
                     float* __restrict dst4x4 ) noexcept;
*/

}
#endif // SOLOSNAKE_MATHS_HPP

