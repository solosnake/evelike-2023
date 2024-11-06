#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_circle.hpp"
#include "solosnake/blue/blue_line_circle_intersection.hpp"
#include "solosnake/blue/blue_line2d.hpp"
#include "solosnake/blue/blue_intersection2d.hpp"

using namespace blue;

namespace
{
    inline float sq( float x ) noexcept
    {
        return x * x;
    }
}

TEST(line_circle_intersection, ctor)
{
    Intersection2d i;
    EXPECT_EQ(0, i.size());
}

TEST(line_circle_intersection, unit_circle_and_y_axis)
{
    Circle C;                          // (0,0) r=1
    Line2d L(0.0f, -1.0f, 0.0f, 1.0f); // x = 0

    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(0.0f, i[0][0]);
        EXPECT_FLOAT_EQ(1.0f, i[0][1]);
        EXPECT_FLOAT_EQ(0.0f, i[1][0]);
        EXPECT_FLOAT_EQ(-1.0f, i[1][1]);
    }
}

TEST(line_circle_intersection, circle_at_53)
{
    Circle C(5.0f, 3.0f, 1.5f);
    Line2d L(5.0f, -1.0f, 5.0f, 1.0f); // x = 5

    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(5.0f, i[0][0]);
        EXPECT_FLOAT_EQ(4.5f, i[0][1]);
        EXPECT_FLOAT_EQ(5.0f, i[1][0]);
        EXPECT_FLOAT_EQ(1.5f, i[1][1]);
    }
}

TEST(line_circle_intersection, circle_at_minus53)
{
    Circle C(-5.0f, 3.0f, 1.5f);
    Line2d L(-5.0f, -1.0f, -5.0f, 1.0f); // x = -5

    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(-5.0f, i[0][0]);
        EXPECT_FLOAT_EQ(4.5f, i[0][1]);
        EXPECT_FLOAT_EQ(-5.0f, i[1][0]);
        EXPECT_FLOAT_EQ(1.5f, i[1][1]);
    }
}

TEST(line_circle_intersection, circle_at_minus50)
{
    Circle C(-5.0f, 0.0f, 1.5f);
    Line2d L(-5.0f, -10.0f, -5.0f, 10.0f); // x = -5

    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(-5.0f, i[0][0]);
        EXPECT_FLOAT_EQ(1.5f, i[0][1]);
        EXPECT_FLOAT_EQ(-5.0f, i[1][0]);
        EXPECT_FLOAT_EQ(-1.5f, i[1][1]);
    }
}

TEST(line_circle_intersection, unit_circle_and_y_equal_zero)
{
    Circle C; // (0,0) r=1
    Line2d L; // y = 0
    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(+1.0f, i[0][0]);
        EXPECT_FLOAT_EQ(0.0f, i[0][1]);
        EXPECT_FLOAT_EQ(-1.0f, i[1][0]);
        EXPECT_FLOAT_EQ(0.0f, i[1][1]);
    }
}

TEST(line_circle_intersection, fortyfive_degree)
{
    const float radius = 1.0f;
    Circle C(2.0f, 2.0f, radius);
    Line2d L(0.0f, 0.0f, 2.0f, 2.0f);
    Intersection2d i = line_circle_intersection(L, C);

    EXPECT_EQ(2, i.size());

    if (i.size() > 0)
    {
        // Slope is 1, c is 0 : y = x + c

        // Check x == y for both solutions.
        EXPECT_FLOAT_EQ(i[0][0], i[0][1]);
        EXPECT_FLOAT_EQ(i[1][0], i[1][1]);

        // Check they fit circle equations:
        EXPECT_FLOAT_EQ(sq(i[0][0] - C.centre()[0]) + sq(i[0][1] - C.centre()[1]), sq(radius));
        EXPECT_FLOAT_EQ(sq(i[1][0] - C.centre()[0]) + sq(i[1][1] - C.centre()[1]), sq(radius));
    }
}

TEST(line_circle_intersection, no_intersection_sloped)
{
    const float radius = 5.0f;
    Circle C(-25.0f, -25.0f, radius);
    Line2d L(-10.0f, -2.0f, 10.0f, 2.0f); // Slight upward slope.

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(0, i.size());
}

TEST(line_circle_intersection, no_intersection_y_axis)
{
    const float radius = 4.9f;
    Circle C(6.0f, 3.0f, radius);
    Line2d L(1.0f, -2.0f, 1.0f, 2.0f); // parallel to y axis special case.

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(0, i.size());
}

TEST(line_circle_intersection, no_intersection_x_axis)
{
    const float radius = 2.9f;
    Circle C(6.0f, 3.0f, radius);
    Line2d L(-1.0f, -1.0f, 1.0f, -1.0f); // parallel to x axis.

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(0, i.size());
}

TEST(line_circle_intersection, one_intersection)
{
    Circle C;
    Line2d L(-1.0f, -1.0f, 1.0f, -1.0f); // parallel to x axis.

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(1, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(0.0f, i[0][0]);
        EXPECT_FLOAT_EQ(-1.0f, i[0][1]);
        // Even though there is only 1 intersection, we are guaranteed that
        // both points will be the same point:
        EXPECT_FLOAT_EQ(0.0f, i[1][0]);
        EXPECT_FLOAT_EQ(-1.0f, i[1][1]);
    }
}

TEST(line_circle_intersection, one_intersection_y_axis)
{
    Circle C; // Unit circle, radius = 1.
    EXPECT_FLOAT_EQ(1.0f, C.radius());

    Line2d L(-1.0f, +1.0f, -1.0f, -1.0f); // parallel to y axis.

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(1, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(-1.0f, i[0][0]);
        EXPECT_FLOAT_EQ(0.0f, i[0][1]);
        // Even though there is only 1 intersection, we are guaranteed that
        // both points will be the same point:
        EXPECT_FLOAT_EQ(-1.0f, i[1][0]);
        EXPECT_FLOAT_EQ(0.0f, i[1][1]);
    }
}

TEST(line_circle_intersection, one_intersection_real)
{
    const float x = +128.75f;
    const float y = -523.45f;
    const float r = 13.9f;
    const float p = 123.4f;
    const float bottom = y - r;

    Circle C(x, y, r);
    Line2d L(p, bottom, -p, bottom);

    Intersection2d i = line_circle_intersection(L, C);
    EXPECT_EQ(1, i.size());

    if (i.size() > 0)
    {
        EXPECT_FLOAT_EQ(x, i[0][0]);
        EXPECT_FLOAT_EQ(bottom, i[0][1]);
        // Even though there is only 1 intersection, we are guaranteed that
        // both points will be the same point:
        EXPECT_FLOAT_EQ(x, i[1][0]);
        EXPECT_FLOAT_EQ(bottom, i[1][1]);
    }
}