#include "solosnake/matrix3d.hpp"

namespace solosnake
{

    //! Returns the determinant of the 3x3 rotational portion of the 4x4 matrix.
    float rotation_determinant_4x4( const float* unaliased m4x4 )
    {
        //  0  1  2
        //  4  5  6
        //  8  9  10
        return   m4x4[0] * ( m4x4[5] * m4x4[10] - m4x4[6] * m4x4[9] )
               - m4x4[1] * ( m4x4[4] * m4x4[10] - m4x4[6] * m4x4[8] )
               + m4x4[2] * ( m4x4[4] * m4x4[ 9] - m4x4[5] * m4x4[8] );
    }

    // This code comes directly from GLU except that it is for float
    bool invert_4x4( const float* unaliased m, float* unaliased dst )
    {
#ifdef SWAP_ROWS_FLOAT
#undef SWAP_ROWS_FLOAT
#endif

#ifdef MAT
#undef MAT
#endif

#define SWAP_ROWS_FLOAT(a, b) { float* _tmp = a;  (a) = (b); (b) = _tmp; }

#define MAT(m, r, c) ((m)[(c) * 4 + (r)])

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

#undef SWAP_ROWS_FLOAT
#undef MAT
    }

    void load_look_at( const float* unaliased lookedAt3f,
                       const float* unaliased position3f,
                       const float* unaliased upVec3f,
                       float* unaliased result4x4 )
    {
        float forward[3];
        float side[3];
        float up[3];
        matrix4x4_t dst4x4;

        forward[0] = lookedAt3f[0] - position3f[0];
        forward[1] = lookedAt3f[1] - position3f[1];
        forward[2] = lookedAt3f[2] - position3f[2];

        normalise3( forward );

        // Side = forward x up
        cross3( forward, upVec3f, side );
        normalise3( side );

        // Recompute up as: up = side x forward
        cross3( side, forward, up );

        dst4x4[0] = side[0];
        dst4x4[1] = up[0];
        dst4x4[2] = -forward[0];
        dst4x4[3] = 0.0f;

        dst4x4[4] = side[1];
        dst4x4[5] = up[1];
        dst4x4[6] = -forward[1];
        dst4x4[7] = 0.0f;

        dst4x4[8] = side[2];
        dst4x4[9] = up[2];
        dst4x4[10] = -forward[2];
        dst4x4[11] = 0.0f;

        dst4x4[12] = 0.0f;
        dst4x4[13] = 0.0f;
        dst4x4[14] = 0.0f;
        dst4x4[15] = 1.0;

        matrix4x4_t trans;
        load_translation_4x4( -position3f[0], -position3f[1], -position3f[2], trans );

        mul_4x4( trans, dst4x4, result4x4 );
    }
}
