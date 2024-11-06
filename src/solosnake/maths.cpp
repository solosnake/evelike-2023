#include "solosnake/maths.hpp"

#ifdef SWAP_ROWS_FLOAT
#   undef SWAP_ROWS_FLOAT
#endif

#ifdef MAT
#   undef MAT
#endif

#define SWAP_ROWS_FLOAT(a, b)   { float* _tmp = a;  (a) = (b); (b) = _tmp; }
#define MAT(m, r, c)            ((m)[(c) * 4 + (r)])

namespace solosnake
{
    void load_look_at( const Point3d* __restrict lookedAt3f,
                       const Point3d* __restrict position3f,
                       const Point3d* __restrict upVec3f,
                       Matrix4x4* __restrict result4x4 ) noexcept
    {
        Point3d forward { lookedAt3f->x - position3f->x,
                          lookedAt3f->y - position3f->y,
                          lookedAt3f->z - position3f->z };

        // Normalise `forward`
        const auto f = std::sqrt( ( forward.x * forward.x )
                                + ( forward.y * forward.y )
                                + ( forward.z * forward.z ) );
        assert( f > 0.0001f );
        forward.x /= f;
        forward.y /= f;
        forward.z /= f;

        // Side = forward x up (cross product).
        Point3d side { forward.y * upVec3f->z - forward.z * upVec3f->y,
                       forward.z * upVec3f->x - forward.x * upVec3f->z, // NOTE -( x * op1->z - z * op1->x )
                       forward.x * upVec3f->y - forward.y * upVec3f->x };

        // Normalise `side`
        const auto g = std::sqrt( ( side.x * side.x )
                                + ( side.y * side.y )
                                + ( side.z * side.z ) );
        assert( g > 0.0001f );
        side.x /= g;
        side.y /= g;
        side.z /= g;

        // Recompute up as: up = side x forward (cross product).
        Point3d up { side.y * forward.z - side.z * forward.y,
                     side.z * forward.x - side.x * forward.z, // NOTE -( x * op1->z - z * op1->x )
                     side.x * forward.y - side.y * forward.x };

        // Normalise `up` (it should be close to unit length already).
        const auto h = std::sqrt( ( up.x * up.x )
                                + ( up.y * up.y )
                                + ( up.z * up.z ) );
        assert( h > 0.0001f );
        up.x /= h;
        up.y /= h;
        up.z /= h;

        Matrix4x4 dst4x4 { side.x,  up.x, -forward.x, 0.0f,
                           side.y,  up.y, -forward.y, 0.0f,
                           side.z,  up.z, -forward.z, 0.0f,
                              0.0,  0.0f,       0.0f, 1.0f };

        Matrix4x4 trans {           1.0f,           0.0f,           0.0f, 0.0f,
                                    0.0f,           1.0f,           0.0f, 0.0f,
                                    0.0f,           0.0f,           1.0f, 0.0f,
                          -position3f->x, -position3f->y, -position3f->z, 1.0f };

        mul_4x4( &dst4x4, &trans, result4x4 );
    }
/*
   /// Returns the determinant of the 3x3 rotational portion of the 4x4 matrix.
    float rotation_determinant_4x4( const float* m4x4 ) noexcept
    {
        //  0  1  2
        //  4  5  6
        //  8  9  10
        return   m4x4[0] * ( m4x4[5] * m4x4[10] - m4x4[6] * m4x4[9] )
               - m4x4[1] * ( m4x4[4] * m4x4[10] - m4x4[6] * m4x4[8] )
               + m4x4[2] * ( m4x4[4] * m4x4[ 9] - m4x4[5] * m4x4[8] );
    }

    // This code comes directly from GLU except that it is for float
    bool invert_4x4( const float* __restrict m, float* __restrict dst ) noexcept
    {
        float wtmp[4][8];

        float* r0 = wtmp[0];
        float* r1 = wtmp[1];
        float* r2 = wtmp[2];
        float* r3 = wtmp[3];

        r0[0] = MAT( m, 0, 0 );
        r0[1] = MAT( m, 0, 1 );
        r0[2] = MAT( m, 0, 2 );
        r0[3] = MAT( m, 0, 3 );

        r0[4] = 1.0f;
        r0[5] = 0.0f;
        r0[6] = 0.0f;
        r0[7] = 0.0f;

        r1[0] = MAT( m, 1, 0 );
        r1[1] = MAT( m, 1, 1 );
        r1[2] = MAT( m, 1, 2 );
        r1[3] = MAT( m, 1, 3 );

        r1[5] = 1.0f;
        r1[4] = 0.0f;
        r1[6] = 0.0f;
        r1[7] = 0.0f;

        r2[0] = MAT( m, 2, 0 );
        r2[1] = MAT( m, 2, 1 );
        r2[2] = MAT( m, 2, 2 );
        r2[3] = MAT( m, 2, 3 );

        r2[6] = 1.0f;
        r2[4] = 0.0f;
        r2[5] = 0.0f;
        r2[7] = 0.0f;

        r3[0] = MAT( m, 3, 0 );
        r3[1] = MAT( m, 3, 1 );
        r3[2] = MAT( m, 3, 2 );
        r3[3] = MAT( m, 3, 3 );

        r3[7] = 1.0f;
        r3[4] = 0.0f;
        r3[5] = 0.0f;
        r3[6] = 0.0f;

        // choose pivot - or die
        if( fabsf( r3[0] ) > fabsf( r2[0] ) )
        {
            SWAP_ROWS_FLOAT( r3, r2 );
        }

        if( fabsf( r2[0] ) > fabsf( r1[0] ) )
        {
            SWAP_ROWS_FLOAT( r2, r1 );
        }

        if( fabsf( r1[0] ) > fabsf( r0[0] ) )
        {
            SWAP_ROWS_FLOAT( r1, r0 );
        }

        if( 0.0f == r0[0] )
        {
            assert( ! "invert_4x4 failed." );
            return false;
        }

        // eliminate first variable
        float m1 = r1[0] / r0[0];
        float m2 = r2[0] / r0[0];
        float m3 = r3[0] / r0[0];
        float s = r0[1];

        r1[1] -= m1 * s;
        r2[1] -= m2 * s;
        r3[1] -= m3 * s;
        s = r0[2];

        r1[2] -= m1 * s;
        r2[2] -= m2 * s;
        r3[2] -= m3 * s;
        s = r0[3];

        r1[3] -= m1 * s;
        r2[3] -= m2 * s;
        r3[3] -= m3 * s;
        s = r0[4];

        if( s != 0.0f )
        {
            r1[4] -= m1 * s;
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }

        s = r0[5];

        if( s != 0.0f )
        {
            r1[5] -= m1 * s;
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }

        s = r0[6];

        if( s != 0.0f )
        {
            r1[6] -= m1 * s;
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }

        s = r0[7];

        if( s != 0.0f )
        {
            r1[7] -= m1 * s;
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        // choose pivot - or die
        if( fabsf( r3[1] ) > fabsf( r2[1] ) )
        {
            SWAP_ROWS_FLOAT( r3, r2 );
        }

        if( fabsf( r2[1] ) > fabsf( r1[1] ) )
        {
            SWAP_ROWS_FLOAT( r2, r1 );
        }

        if( 0.0f == r1[1] )
        {
            assert( ! "invert_4x4 failed." );
            return false;
        }

        // eliminate second variable
        m2 = r2[1] / r1[1];
        m3 = r3[1] / r1[1];

        r2[2] -= m2 * r1[2];
        r3[2] -= m3 * r1[2];
        r2[3] -= m2 * r1[3];
        r3[3] -= m3 * r1[3];

        s = r1[4];

        if( 0.0f != s )
        {
            r2[4] -= m2 * s;
            r3[4] -= m3 * s;
        }

        s = r1[5];

        if( 0.0f != s )
        {
            r2[5] -= m2 * s;
            r3[5] -= m3 * s;
        }

        s = r1[6];

        if( 0.0f != s )
        {
            r2[6] -= m2 * s;
            r3[6] -= m3 * s;
        }

        s = r1[7];

        if( 0.0f != s )
        {
            r2[7] -= m2 * s;
            r3[7] -= m3 * s;
        }

        // choose pivot - or die
        if( fabsf( r3[2] ) > fabsf( r2[2] ) )
        {
            SWAP_ROWS_FLOAT( r3, r2 );
        }

        if( 0.0f == r2[2] )
        {
            assert( ! "invert_4x4 failed." );
            return false;
        }

        // eliminate third variable
        m3 = r3[2] / r2[2];
        r3[3] -= m3 * r2[3];
        r3[4] -= m3 * r2[4];
        r3[5] -= m3 * r2[5];
        r3[6] -= m3 * r2[6];
        r3[7] -= m3 * r2[7];

        // last check
        if( 0.0f == r3[3] )
        {
            assert( ! "invert_4x4 failed." );
            return false;
        }

        s = 1.0f / r3[3]; // now back substitute row 3

        r3[4] *= s;
        r3[5] *= s;
        r3[6] *= s;
        r3[7] *= s;

        m2 = r2[3]; // now back substitute row 2

        s = 1.0f / r2[2];

        r2[4] = s * ( r2[4] - r3[4] * m2 );
        r2[5] = s * ( r2[5] - r3[5] * m2 );
        r2[6] = s * ( r2[6] - r3[6] * m2 );
        r2[7] = s * ( r2[7] - r3[7] * m2 );

        m1 = r1[3];

        r1[4] -= r3[4] * m1;
        r1[5] -= r3[5] * m1;
        r1[6] -= r3[6] * m1;
        r1[7] -= r3[7] * m1;

        float m0 = r0[3];

        r0[4] -= r3[4] * m0;
        r0[5] -= r3[5] * m0;
        r0[6] -= r3[6] * m0;
        r0[7] -= r3[7] * m0;

        m1 = r1[2]; // now back substitute row 1

        s = 1.0f / r1[1];

        r1[4] = s * ( r1[4] - r2[4] * m1 );
        r1[5] = s * ( r1[5] - r2[5] * m1 );
        r1[6] = s * ( r1[6] - r2[6] * m1 );
        r1[7] = s * ( r1[7] - r2[7] * m1 );

        m0 = r0[2];

        r0[4] -= r2[4] * m0;
        r0[5] -= r2[5] * m0;
        r0[6] -= r2[6] * m0;
        r0[7] -= r2[7] * m0;

        m0 = r0[1]; // now back substitute row 0

        s = 1.0f / r0[0];

        r0[4] = s * ( r0[4] - r1[4] * m0 );
        r0[5] = s * ( r0[5] - r1[5] * m0 );
        r0[6] = s * ( r0[6] - r1[6] * m0 );
        r0[7] = s * ( r0[7] - r1[7] * m0 );

        MAT( dst, 0, 0 ) = r0[4];
        MAT( dst, 0, 1 ) = r0[5];
        MAT( dst, 0, 2 ) = r0[6];
        MAT( dst, 0, 3 ) = r0[7];
        MAT( dst, 1, 0 ) = r1[4];
        MAT( dst, 1, 1 ) = r1[5];
        MAT( dst, 1, 2 ) = r1[6];
        MAT( dst, 1, 3 ) = r1[7];
        MAT( dst, 2, 0 ) = r2[4];
        MAT( dst, 2, 1 ) = r2[5];
        MAT( dst, 2, 2 ) = r2[6];
        MAT( dst, 2, 3 ) = r2[7];
        MAT( dst, 3, 0 ) = r3[4];
        MAT( dst, 3, 1 ) = r3[5];
        MAT( dst, 3, 2 ) = r3[6];
        MAT( dst, 3, 3 ) = r3[7];

        return true;
    }
*/

}

#undef SWAP_ROWS_FLOAT
#undef MAT
