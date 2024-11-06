#include "solosnake/testing/testing.hpp"
#include "solosnake/maths.hpp"


// OpenGL:
//        +Y
//          |
//          |__ __ +X
//         /
//    +Z /
//
// OpenGL is right-handed: the positive x and y axes point right and up, and
//  the negative z axis points forward. Positive rotation is counter-clockwise
// about the axis of rotation.
TEST( Maths, mul_4x4 )
{
    solosnake::Matrix4x4 m1 {
        1.0f, 2.0f, 3.0f, 4.0f, // m11 m21 m31 m41
        5.0f, 6.0f, 7.0f, 8.0f, // m21 m22 m23 m24
        9.0f, 0.0f, 1.0f, 2.0f, // m31 m32 m33 m34
        3.0f, 4.0f, 5.0f, 6.0f  // m41 m42 m43 m44
    };

    solosnake::Matrix4x4 m2 {
        2.0f, 3.0f, 4.0f, 5.0f,
        3.0f, 4.0f, 5.0f, 6.0f,
        4.0f, 5.0f, 6.0f, 7.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };

    solosnake::Matrix4x4 result {};

    solosnake::mul_4x4(&m1, &m2, &result);

    EXPECT_FLOAT_EQ( result.m11,   68.0f );
    EXPECT_FLOAT_EQ( result.m12,   86.0f );
    EXPECT_FLOAT_EQ( result.m13,  104.0f );
    EXPECT_FLOAT_EQ( result.m14,  122.0f );

    EXPECT_FLOAT_EQ( result.m21,   42.0f );
    EXPECT_FLOAT_EQ( result.m22,   54.0f );
    EXPECT_FLOAT_EQ( result.m23,   66.0f );
    EXPECT_FLOAT_EQ( result.m24,   78.0f );

    EXPECT_FLOAT_EQ( result.m31,   56.0f );
    EXPECT_FLOAT_EQ( result.m32,   72.0f );
    EXPECT_FLOAT_EQ( result.m33,   88.0f );
    EXPECT_FLOAT_EQ( result.m34,  104.0f );

    EXPECT_FLOAT_EQ( result.m41,  70.0f );
    EXPECT_FLOAT_EQ( result.m42,  90.0f );
    EXPECT_FLOAT_EQ( result.m43, 110.0f );
    EXPECT_FLOAT_EQ( result.m44, 130.0f );

    result = solosnake::mul_4x4(&m1, &m2);

    EXPECT_FLOAT_EQ( result.m11,   68.0f );
    EXPECT_FLOAT_EQ( result.m12,   86.0f );
    EXPECT_FLOAT_EQ( result.m13,  104.0f );
    EXPECT_FLOAT_EQ( result.m14,  122.0f );

    EXPECT_FLOAT_EQ( result.m21,   42.0f );
    EXPECT_FLOAT_EQ( result.m22,   54.0f );
    EXPECT_FLOAT_EQ( result.m23,   66.0f );
    EXPECT_FLOAT_EQ( result.m24,   78.0f );

    EXPECT_FLOAT_EQ( result.m31,   56.0f );
    EXPECT_FLOAT_EQ( result.m32,   72.0f );
    EXPECT_FLOAT_EQ( result.m33,   88.0f );
    EXPECT_FLOAT_EQ( result.m34,  104.0f );

    EXPECT_FLOAT_EQ( result.m41,  70.0f );
    EXPECT_FLOAT_EQ( result.m42,  90.0f );
    EXPECT_FLOAT_EQ( result.m43, 110.0f );
    EXPECT_FLOAT_EQ( result.m44, 130.0f );
}

TEST( Maths, mul_p4_by_identity )
{
    solosnake::Point4d point{ -1.5, 2.0f, 3.0f, 1.0f };
    solosnake::Matrix4x4 m = solosnake::Identity4x4();
    auto p2 = solosnake::mul_4x1(&m, &point);
    EXPECT_FLOAT_EQ( point.x, -1.5f );
    EXPECT_FLOAT_EQ( point.y,  2.0f );
    EXPECT_FLOAT_EQ( point.z,  3.0f );
    EXPECT_FLOAT_EQ( point.w,  1.0f );
}

TEST( Maths, mul_p4_by_rotation )
{
    constexpr auto angle = solosnake::Pi / 2.0f; // 90 degrees.
    solosnake::Point4d point{ 2.0f, 0.0f, 0.0f, 1.0f };
    solosnake::Matrix4x4 r = solosnake::rotation_4x4_y( angle );
    auto p2 = solosnake::mul_4x1(&r, &point);
    // Rotate (2,0,0) about Y axis gives (0,0,-2).
    EXPECT_FLOAT_EQ( p2.x,  0.0f );
    EXPECT_FLOAT_EQ( p2.y,  0.0f );
    EXPECT_FLOAT_EQ( p2.z, -2.0f );
    EXPECT_FLOAT_EQ( p2.w,  1.0f );
}

TEST( Maths, mul_p4_by_translate_XYZ )
{
    solosnake::Point4d point{ 0.0f, 0.0f, 0.0f, 1.0f };
    solosnake::Matrix4x4 m = solosnake::translation_4x4( 10.0f, -20.0f, 35.0f );
    auto p2 = solosnake::mul_4x1(&m, &point);
    EXPECT_FLOAT_EQ( p2.x,  10.0f );
    EXPECT_FLOAT_EQ( p2.y, -20.0f );
    EXPECT_FLOAT_EQ( p2.z,  35.0f );
    EXPECT_FLOAT_EQ( p2.w,   1.0f );
}