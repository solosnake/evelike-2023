#define RUN_TESTS

#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include "solosnake/linesegment.hpp"

using namespace solosnake;

namespace
{

static inline bool close_enough(float a, float b)
{
    return std::abs(a - b) < 0.0011f;
}
}

TEST(linesegment2d, linesegment2d)
{
    point2d p0(1.0f, 2.0f);
    point2d p1(4.0f, 5.0f);

    linesegment2d ls(p0, p1);
    linesegment2d ls2(p0, p1);
    linesegment2d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_TRUE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_FALSE(ls3 != ls2);
}

TEST(linesegment3d, linesegment3d)
{
    point3d p0(1.0f, 2.0f, 3.0f);
    point3d p1(4.0f, 5.0f, 6.0f);

    linesegment3d ls(p0, p1);
    linesegment3d ls2(p0, p1);
    linesegment3d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_TRUE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_FALSE(ls3 != ls2);
}

TEST(linesegment4d, linesegment4d)
{
    point4d p0(1.0f, 2.0f, 3.0f, 4.0f);
    point4d p1(4.0f, 5.0f, 6.0f, 7.0f);

    linesegment4d ls(p0, p1);
    linesegment4d ls2(p0, p1);
    linesegment4d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_TRUE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_FALSE(ls3 != ls2);
}

TEST(directedlinesegment2d, linesegment2d)
{
    point2d p0(1.0f, 2.0f);
    point2d p1(4.0f, 5.0f);

    directedlinesegment2d ls(p0, p1);
    directedlinesegment2d ls2(p0, p1);
    directedlinesegment2d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_FALSE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_TRUE(ls3 != ls2);
}

TEST(directedlinesegment3d, linesegment3d)
{
    point3d p0(1.0f, 2.0f, 3.0f);
    point3d p1(4.0f, 5.0f, 6.0f);

    directedlinesegment3d ls(p0, p1);
    directedlinesegment3d ls2(p0, p1);
    directedlinesegment3d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_FALSE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_TRUE(ls3 != ls2);
}

TEST(directedlinesegment4d, linesegment4d)
{
    point4d p0(1.0f, 2.0f, 3.0f, 4.0f);
    point4d p1(4.0f, 5.0f, 6.0f, 7.0f);

    directedlinesegment4d ls(p0, p1);
    directedlinesegment4d ls2(p0, p1);
    directedlinesegment4d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls.tail() == p1);
    EXPECT_TRUE(ls == ls2);
    EXPECT_FALSE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_TRUE(ls3 != ls2);
}

TEST(halfline3d, halfline3d)
{
    point3d p0(1.0f, 2.0f, 3.0f);
    point3d p1(4.0f, 5.0f, 6.0f);

    halfline3d ls(p0, p1);
    halfline3d ls2(p0, p1);
    halfline3d ls3(p1, p0);

    EXPECT_TRUE(ls.head() == p0);
    EXPECT_TRUE(ls == ls2);
    EXPECT_FALSE(ls3 == ls2);
    EXPECT_FALSE(ls != ls2);
    EXPECT_TRUE(ls3 != ls2);
}

TEST(halfline3d, line_plane_3d)
{
    {
        point3d p0(0.0f, +10.0f, 0.0f);
        point3d p1(0.0f, -10.0f, 0.0f);
        halfline3d line(p0, p1);
        float y = 0.0f;

        float result[3] = { 0.0f };
        EXPECT_TRUE(find_halfline3d_yplane_intersection(line, y, result));
        EXPECT_EQ(0.0, result[0]);
        EXPECT_EQ(y, result[1]);
        EXPECT_EQ(0.0, result[2]);
    }

    {
        point3d p0(0.0f, +10.0f, 0.0f);
        point3d p1(0.0f, -10.0f, 0.0f);
        halfline3d line(p0, p1);
        float y = 5.0f;

        float result[3] = { 0.0f };
        EXPECT_TRUE(find_halfline3d_yplane_intersection(line, y, result));
        EXPECT_EQ(0.0, result[0]);
        EXPECT_EQ(y, result[1]);
        EXPECT_EQ(0.0, result[2]);
    }

    {
        point3d p0(0.0f, +10.0f, +10.0f);
        point3d p1(0.0f, -10.0f, -10.0f);
        halfline3d line(p0, p1);
        float y = 0.0f;

        float result[3] = { 0.0f };
        EXPECT_TRUE(find_halfline3d_yplane_intersection(line, y, result));
        EXPECT_EQ(0.0, result[0]);
        EXPECT_EQ(y, result[1]);
        EXPECT_EQ(0.0, result[2]);
    }

    {
        point3d p0(0.0f, +10.0f, +10.0f);
        point3d p1(0.0f, +20.0f, +5.0f);
        halfline3d line(p0, p1);
        float y = 0.0f;

        float result[3] = { 0.0f };
        EXPECT_FALSE(find_halfline3d_yplane_intersection(line, y, result));
    }
}

TEST(halfline3d, odd_results)
{
    // The following was observed. Note the different Z values for similar
    // inputs:
    //                                                                                                           ???????
    // head: +3.9907 +10.4525 +14.1175 tail: +3.8287 +9.8616 +13.3272 X +224 Y
    // +162 Plane pick 1 +1.1246 -0.0000 +0.1388
    // head: +4.0004 +10.4525 +13.9688 tail: +3.8400 +9.8614 +13.1783 X +225 Y
    // +162 Plane pick 1 +1.1642 +0.0000 -0.0099

    const point3d p0Head(+3.9907f, +10.4525f, +14.1175f);
    const point3d p1Head(+4.0004f, +10.4525f, +13.9688f);

    const point3d p0Tail(+3.8287f, +9.8616f, +13.3272f);
    const point3d p1Tail(+3.8400f, +9.8614f, +13.1783f);

    const halfline3d line0(p0Head, p0Tail);
    const halfline3d line1(p1Head, p1Tail);

    float result0[3] = { 0.0f };
    float result1[3] = { 0.0f };

    const float boardY = 0.0f;

    EXPECT_TRUE(find_halfline3d_yplane_intersection(line0, boardY, result0));
    EXPECT_TRUE(find_halfline3d_yplane_intersection(line1, boardY, result1));

    EXPECT_TRUE(close_enough(+1.1246f, result0[0]));
    EXPECT_TRUE(close_enough(-0.0000f, result0[1]));
    EXPECT_TRUE(close_enough(+0.1388f, result0[2]));

    EXPECT_TRUE(close_enough(+1.1642f, result1[0]));
    EXPECT_TRUE(close_enough(+0.0000f, result1[1]));
    EXPECT_TRUE(close_enough(-0.0099f, result1[2]));
}

#endif // RUN_TESTS