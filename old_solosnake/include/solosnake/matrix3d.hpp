#ifndef solosnake_matrix3d_hpp
#define solosnake_matrix3d_hpp

#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <memory>
#include "solosnake/angles.hpp"
#include "solosnake/point.hpp"
#include "solosnake/unaliased.hpp"
#include "solosnake/inline.hpp"

namespace solosnake
{
    typedef float matrix3x3_t[9];
    typedef float matrix4x4_t[16];

    SS_INLINE float x_4x4( const float* unaliased m4x4 )
    {
        return m4x4[12];
    }

    SS_INLINE float y_4x4( const float* unaliased m4x4 )
    {
        return m4x4[13];
    }

    SS_INLINE float z_4x4( const float* unaliased m4x4 )
    {
        return m4x4[14];
    }

    SS_INLINE Point3d up_4x4( const float* unaliased m4x4 )
    {
        return Point3d( m4x4[4], m4x4[5], m4x4[6] );
    }

    SS_INLINE Point3d down_4x4( const float* unaliased m4x4 )
    {
        return Point3d( -m4x4[4], -m4x4[5], -m4x4[6] );
    }

    SS_INLINE Point3d left_4x4( const float* unaliased m4x4 )
    {
        return Point3d( m4x4[0], m4x4[1], m4x4[2] );
    }

    SS_INLINE Point3d right_4x4( const float* unaliased m4x4 )
    {
        return Point3d( -m4x4[0], -m4x4[1], -m4x4[2] );
    }

    SS_INLINE Point3d forward_4x4( const float* unaliased m4x4 )
    {
        return Point3d( m4x4[8], m4x4[9], m4x4[10] );
    }

    SS_INLINE Point3d backward_4x4( const float* unaliased m4x4 )
    {
        return Point3d( -m4x4[8], -m4x4[9], -m4x4[10] );
    }

    SS_INLINE Point3d location_4x4( const float* unaliased m4x4 )
    {
        return Point3d( m4x4[12], m4x4[13], m4x4[14] );
    }

    SS_INLINE void load_translation_4x4( float x, float y, float z, float* unaliased dst )
    {
        dst[0] = 1.0f;
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;
        dst[4] = 0.0f;
        dst[5] = 1.0f;
        dst[6] = 0.0f;
        dst[7] = 0.0f;
        dst[8] = 0.0f;
        dst[9] = 0.0f;
        dst[10] = 1.0f;
        dst[11] = 0.0f;
        dst[12] = x;
        dst[13] = y;
        dst[14] = z;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_translation_4x4( const float* unaliased point3, float* unaliased dst )
    {
        dst[0] = 1.0f;
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;
        dst[4] = 0.0f;
        dst[5] = 1.0f;
        dst[6] = 0.0f;
        dst[7] = 0.0f;
        dst[8] = 0.0f;
        dst[9] = 0.0f;
        dst[10] = 1.0f;
        dst[11] = 0.0f;
        dst[12] = point3[0];
        dst[13] = point3[1];
        dst[14] = point3[2];
        dst[15] = 1.0f;
    }

    SS_INLINE void set_translation_4x4( const float* unaliased point3, float* unaliased dst )
    {
        dst[12] = point3[0];
        dst[13] = point3[1];
        dst[14] = point3[2];
    }

    SS_INLINE void set_translation_4x4( float x, float y, float z, float* unaliased dst )
    {
        dst[12] = x;
        dst[13] = y;
        dst[14] = z;
    }

    //! For programming purposes, OpenGL matrices are 16-value arrays with base
    //! vectors laid out contiguously in memory. The translation components occupy
    //! the 13th, 14th, and 15th elements of the 16-element matrix, where indices
    //! are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1
    //! Specification.
    SS_INLINE void transpose_4x4( const float* unaliased src, float* unaliased dst )
    {
        dst[0] = src[0];
        dst[1] = src[4];
        dst[2] = src[8];
        dst[3] = src[12];

        dst[4] = src[1];
        dst[5] = src[5];
        dst[6] = src[9];
        dst[7] = src[13];

        dst[8] = src[2];
        dst[9] = src[6];
        dst[10] = src[10];
        dst[11] = src[14];

        dst[12] = src[3];
        dst[13] = src[7];
        dst[14] = src[11];
        dst[15] = src[15];
    }

    //! dst (XYZW) = src (XYZW vector) * m (4x4 matrix)
    //! This would greatly benefit from the array being arranged in
    //! the other direction (SIMD).
    SS_INLINE void transform_4x1( const float* unaliased src,
                                  const float* unaliased m4x4,
                                  float* unaliased dst )
    {
        dst[0] = src[0] * m4x4[0] + src[1] * m4x4[4] + src[2] * m4x4[8] + src[3] * m4x4[12];
        dst[1] = src[0] * m4x4[1] + src[1] * m4x4[5] + src[2] * m4x4[9] + src[3] * m4x4[13];
        dst[2] = src[0] * m4x4[2] + src[1] * m4x4[6] + src[2] * m4x4[10] + src[3] * m4x4[14];
        dst[3] = src[0] * m4x4[3] + src[1] * m4x4[7] + src[2] * m4x4[11] + src[3] * m4x4[15];

        /*
            __m128 x =  _mm_loadu_ps((const float*)&in_x);
            __m128 A0 = _mm_loadu_ps((const float*)(in_A + 0));
            __m128 A1 = _mm_loadu_ps((const float*)(in_A + 1));
            __m128 A2 = _mm_loadu_ps((const float*)(in_A + 2));
            __m128 A3 = _mm_loadu_ps((const float*)(in_A + 3));

            // Multiply each matrix row with the vector x
            __m128 m0 = _mm_mul_ps(A0, x);
            __m128 m1 = _mm_mul_ps(A1, x);
            __m128 m2 = _mm_mul_ps(A2, x);
            __m128 m3 = _mm_mul_ps(A3, x);

            // Using HADD, we add four floats at a time
            __m128 sum_01 = _mm_hadd_ps(m0, m1);
            __m128 sum_23 = _mm_hadd_ps(m2, m3);
            __m128 result = _mm_hadd_ps(sum_01, sum_23);

            // Finally, store the result
            _mm_storeu_ps((float*)&out_y, result);#
            */
    }

    //! dst (4x4) = lhs (4x4) x rhs (4x4)
    //! In terms of perceived effect, rhs happens after lhs.
    SS_INLINE void mul_4x4( const float* unaliased lhs, const float* unaliased rhs, float* unaliased dst )
    {
        float s[16];
        transpose_4x4( rhs, s );

        for( size_t i = 0; i < 16; i += 4 )
        {
            const float* a = lhs + i;

            dst[i] = a[0] * s[0] + a[1] * s[1] + a[2] * s[2] + a[3] * s[3];
            dst[1 + i] = a[0] * s[4] + a[1] * s[5] + a[2] * s[6] + a[3] * s[7];
            dst[2 + i] = a[0] * s[8] + a[1] * s[9] + a[2] * s[10] + a[3] * s[11];
            dst[3 + i] = a[0] * s[12] + a[1] * s[13] + a[2] * s[14] + a[3] * s[15];
        }
    }

    SS_INLINE void copy_4x4( const float* unaliased src, float* unaliased dst )
    {
        ASSERT_UNALIASED( src, dst, 16, 16 );

        std::memcpy( dst, src, 16 * sizeof( float ) );
    }

    SS_INLINE void load_identity_4x4( float* unaliased dst )
    {
        dst[0] = 1.0f;
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;
        dst[4] = 0.0f;
        dst[5] = 1.0f;
        dst[6] = 0.0f;
        dst[7] = 0.0f;
        dst[8] = 0.0f;
        dst[9] = 0.0f;
        dst[10] = 1.0f;
        dst[11] = 0.0f;
        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    //! Typically, the matrix mode is GL_PROJECTION, and (left bottom -nearVal)
    //! and (right top -nearVal) specify the points on the near clipping plane
    //! that are mapped to the lower left and upper right corners of the window,
    //! respectively, assuming that the eye is located at (0, 0, 0).
    //! -farVal specifies the location of the far clipping plane.
    //! Both nearVal and farVal can be either positive or negative.
    //! http://msdn.microsoft.com/en-us/library/dd373965%28v=vs.85%29.aspx
    //! http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
    SS_INLINE void load_ortho( float left,
                               float right,
                               float bottom,
                               float top,
                               float nearVal,
                               float farVal,
                               float* unaliased dst )
    {
        assert( std::fabs( right - left ) > ( 100 * std::numeric_limits<float>::epsilon() ) );
        assert( std::fabs( top - bottom ) > ( 100 * std::numeric_limits<float>::epsilon() ) );
        assert( std::fabs( farVal - nearVal ) > ( 100 * std::numeric_limits<float>::epsilon() ) );

        dst[0] = 2.0f / ( right - left );
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;

        dst[4] = 0.0f;
        dst[5] = 2.0f / ( top - bottom );
        dst[6] = 0.0f;
        dst[7] = 0.0f;

        dst[8] = 0.0f;
        dst[9] = 0.0f;
        dst[10] = -2.0f / ( farVal - nearVal );
        dst[11] = 0.0f;

        dst[12] = -( right + left ) / ( right - left );
        dst[13] = -( top + bottom ) / ( top - bottom );
        dst[15] = 1.0f;
        dst[14] = -( farVal + nearVal ) / ( farVal - nearVal );
    }

    //! Returns the FOV angle in the x axis based on a FOV in the Y and aspect
    //! ratio of a view.
    SS_INLINE Radians fovx( Radians fovy, float aspect )
    {
        return Radians( 2.0f * std::atan( aspect * std::tan( fovy.value() * 0.5f ) ) );
    }

    //! fovy:    Specifies the total field of view angle, in RADIANS, in the y direction.
    //! aspect:  Specifies the aspect ratio that determines the field of view in the x direction.
    //!          The aspect ratio is the ratio of x (width) to y (height) = (W/H)
    //! zNear:   Specifies the distance from the viewer to the near clipping plane (always positive).
    //! zFar:    Specifies the distance from the viewer to the far clipping plane (always positive).
    SS_INLINE void load_perspective( Radians fovy,
                                     float aspect,
                                     float zNear,
                                     float zFar,
                                     float* unaliased dst )
    {
        assert( zNear > 0.0f );
        assert( zFar > 0.0f );
        assert( std::abs( aspect ) > std::numeric_limits<float>::epsilon() );
        assert( fovy.value() > std::numeric_limits<float>::epsilon() );

        dst[5] = 1.0f / std::tan( fovy.value() * 0.5f );

        dst[0] = dst[5] / aspect;
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;

        dst[4] = 0.0f;

        dst[6] = 0.0f;
        dst[7] = 0.0f;

        dst[8] = 0.0f;
        dst[9] = 0.0f;
        dst[10] = ( zFar + zNear ) / ( zNear - zFar );
        dst[11] = -1.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = ( 2.0f * zFar * zNear ) / ( zNear - zFar );
        dst[15] = 0.0f;
    }

    //! http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    SS_INLINE void load_rotation_3x3_unitaxis_angle( const Radians& t,
                                                     const float * const unaliased axis,
                                                     float* unaliased dst )
    {
        const float cos0   = t.cos();
        const float sin0   = t.sin();
        const float l_cos0 = 1.0f - cos0;

        dst[0] = axis[0] * axis[0] * l_cos0 + cos0;
        dst[1] = axis[0] * axis[1] * l_cos0 - axis[2] * sin0;
        dst[2] = axis[0] * axis[2] * l_cos0 + axis[1] * sin0;

        dst[3] = axis[1] * axis[0] * l_cos0 + axis[2] * sin0;
        dst[4] = axis[1] * axis[1] * l_cos0 + cos0;
        dst[5] = axis[1] * axis[2] * l_cos0 - axis[0] * sin0;

        dst[6] = axis[2] * axis[0] * l_cos0 - axis[1] * sin0;
        dst[7] = axis[2] * axis[1] * l_cos0 + axis[0] * sin0;
        dst[8] = axis[2] * axis[2] * l_cos0 + cos0;
    }

    SS_INLINE void load_rotation_x_3x3( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = 1.0f;
        dst[1] = 0.0f;
        dst[2] = 0.0f;

        dst[3] = 0.0f;
        dst[4] = c;
        dst[5] = -s;

        dst[6] = 0.0f;
        dst[7] = s;
        dst[8] = c;
    }

    SS_INLINE void load_rotation_x_4x4( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = 1.0f;
        dst[1] = 0.0f;
        dst[2] = 0.0f;
        dst[3] = 0.0f;

        dst[4] = 0.0f;
        dst[5] = c;
        dst[6] = -s;
        dst[7] = 0.0f;

        dst[8] = 0.0f;
        dst[9] = s;
        dst[10] = c;
        dst[11] = 0.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_rotation_y_3x3( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = c;
        dst[1] = 0.0f;
        dst[2] = s;

        dst[3] = 0.0f;
        dst[4] = 1.0;
        dst[5] = 0.0;

        dst[6] = -s;
        dst[7] = 0.0;
        dst[8] = c;
    }

    SS_INLINE void load_rotation_y_4x4( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = c;
        dst[1] = 0.0f;
        dst[2] = s;
        dst[3] = 0.0f;

        dst[4] = 0.0f;
        dst[5] = 1.0;
        dst[6] = 0.0;
        dst[7] = 0.0f;

        dst[8] = -s;
        dst[9] = 0.0;
        dst[10] = c;
        dst[11] = 0.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_rotation_z_3x3( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = c;
        dst[1] = -s;
        dst[2] = 0.0f;

        dst[3] = s;
        dst[4] = c;
        dst[5] = 0.0;

        dst[6] = 0.0f;
        dst[7] = 0.0;
        dst[8] = 1.0f;
    }

    SS_INLINE void load_rotation_z_4x4( Radians t, float* unaliased dst )
    {
        float c = t.cos();
        float s = t.sin();

        dst[0] = c;
        dst[1] = -s;
        dst[2] = 0.0f;
        dst[3] = 0.0f;

        dst[4] = s;
        dst[5] = c;
        dst[6] = 0.0;
        dst[7] = 0.0f;

        dst[8] = 0.0f;
        dst[9] = 0.0;
        dst[10] = 1.0f;
        dst[11] = 0.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_rotation_xyz_4x4( Radians x, Radians y, Radians z, float* unaliased dst )
    {
        float cx = x.cos();
        float cy = y.cos();
        float cz = z.cos();
        float sx = x.sin();
        float sy = y.sin();
        float sz = z.sin();

        dst[0] = cy * cz;
        dst[1] = ( sx * sy * cz ) - ( cx * sz );
        dst[2] = ( sx * sz ) + ( cx * sy * cz );
        dst[3] = 0.0f;

        dst[4] = cy * sz;
        dst[5] = ( cx * cz ) + ( sx * sy * sz );
        dst[6] = ( cx * sy * sz ) - ( sx * cz );
        dst[7] = 0.0f;

        dst[8] = -sy;
        dst[9] = sx * cy;
        dst[10] = cx * cy;
        dst[11] = 0.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_rotation_xy_4x4( Radians x, Radians y, float* unaliased dst )
    {
        float cx = x.cos();
        float cy = y.cos();
        float sx = x.sin();
        float sy = y.sin();

        dst[0] = cy;
        dst[1] = sx * sy;
        dst[2] = cx * sy;
        dst[3] = 0.0f;

        dst[4] = 0.0f;
        dst[5] = cx;
        dst[6] = -sx;
        dst[7] = 0.0f;

        dst[8] = -sy;
        dst[9] = sx * cy;
        dst[10] = cx * cy;
        dst[11] = 0.0f;

        dst[12] = 0.0f;
        dst[13] = 0.0f;
        dst[14] = 0.0f;
        dst[15] = 1.0f;
    }

    SS_INLINE void load_rotation_xy_3x3( Radians x, Radians y, float* unaliased dst )
    {
        float cx = x.cos();
        float cy = y.cos();
        float sx = x.sin();
        float sy = y.sin();

        dst[0] = cy;
        dst[1] = sx * sy;
        dst[2] = cx * sy;

        dst[3] = 0.0f;
        dst[4] = cx;
        dst[5] = -sx;

        dst[6] = -sy;
        dst[7] = sx * cy;
        dst[8] = cx * cy;
    }

    SS_INLINE void load_xyz_ry_4x4( float x, float y, float z, Radians ry, float* unaliased dst )
    {
        load_rotation_y_4x4( ry, dst );
        set_translation_4x4( x, y, z, dst );
    }

    SS_INLINE void load_copy_4x4( const float* src, float* dst )
    {
        std::memcpy( dst, src, sizeof( float ) * 16 );
    }

    SS_INLINE bool identifical_4x4( const float* unaliased lhs, const float* unaliased rhs )
    {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3]
               && lhs[4] == rhs[4] && lhs[5] == rhs[5] && lhs[6] == rhs[6] && lhs[7] == rhs[7]
               && lhs[8] == rhs[8] && lhs[9] == rhs[9] && lhs[10] == rhs[10] && lhs[11] == rhs[11]
               && lhs[12] == rhs[12] && lhs[13] == rhs[13] && lhs[14] == rhs[14] && lhs[15] == rhs[15];
    }

    float rotation_determinant_4x4( const float* unaliased m4x4 );

    bool invert_4x4( const float* unaliased src, float* unaliased dst );

    void load_look_at( const float* unaliased lookedAt3f,
                       const float* unaliased position3f,
                       const float* unaliased upVec3f,
                       float* unaliased result );
}
#endif // matrix3d_hpp
