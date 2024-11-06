#include "solosnake/testing/testing.hpp"
#include "solosnake/point.hpp"

using namespace solosnake;

const point2d p2d_a1(1.0f, 2.0f);
const point2d p2d_a2(1.0f, 2.0f);
const point2d p2d_b1(5.0f, 6.0f);
const point2d p2d_b2(5.0f, 6.0f);

const point3d p3d_a1(1.0f, 2.0f, 3.0f);
const point3d p3d_a2(1.0f, 2.0f, 3.0f);
const point3d p3d_b1(5.0f, 6.0f, 7.0f);
const point3d p3d_b2(5.0f, 6.0f, 7.0f);

const point4d p4d_a1(1.0f, 2.0f, 3.0f, 4.0f);
const point4d p4d_a2(1.0f, 2.0f, 3.0f, 4.0f);
const point4d p4d_b1(5.0f, 6.0f, 7.0f, 8.0f);
const point4d p4d_b2(5.0f, 6.0f, 7.0f, 8.0f);

template <typename POINTTYPE> POINTTYPE pa1();
template <typename POINTTYPE> POINTTYPE pa2();
template <typename POINTTYPE> POINTTYPE pb1();
template <typename POINTTYPE> POINTTYPE pb2();

template <> point2d pa1<point2d>()
{
    return p2d_a1;
}
template <> point2d pa2<point2d>()
{
    return p2d_a2;
}
template <> point2d pb1<point2d>()
{
    return p2d_b1;
}
template <> point2d pb2<point2d>()
{
    return p2d_b2;
}

template <> point3d pa1<point3d>()
{
    return p3d_a1;
}
template <> point3d pa2<point3d>()
{
    return p3d_a2;
}
template <> point3d pb1<point3d>()
{
    return p3d_b1;
}
template <> point3d pb2<point3d>()
{
    return p3d_b2;
}

template <> point4d pa1<point4d>()
{
    return p4d_a1;
}
template <> point4d pa2<point4d>()
{
    return p4d_a2;
}
template <> point4d pb1<point4d>()
{
    return p4d_b1;
}
template <> point4d pb2<point4d>()
{
    return p4d_b2;
}

template <typename POINTTYPE> void do_comparison_tests()
{
    EXPECT_TRUE(pa1<POINTTYPE>() == pa2<POINTTYPE>());
    EXPECT_FALSE(pa1<POINTTYPE>() != pa2<POINTTYPE>());
    EXPECT_TRUE(pa1<POINTTYPE>() >= pa2<POINTTYPE>());
    EXPECT_TRUE(pa1<POINTTYPE>() <= pa2<POINTTYPE>());
    EXPECT_FALSE(pa1<POINTTYPE>() < pa2<POINTTYPE>());
    EXPECT_FALSE(pa1<POINTTYPE>() > pa2<POINTTYPE>());

    EXPECT_FALSE(pa1<POINTTYPE>() == pb1<POINTTYPE>());
    EXPECT_TRUE(pa1<POINTTYPE>() != pb1<POINTTYPE>());
    EXPECT_TRUE(pb1<POINTTYPE>() >= pa1<POINTTYPE>());
    EXPECT_TRUE(pa1<POINTTYPE>() <= pb1<POINTTYPE>());
    EXPECT_TRUE(pa1<POINTTYPE>() < pb1<POINTTYPE>());
    EXPECT_TRUE(pb1<POINTTYPE>() > pa1<POINTTYPE>());
}

TEST(point, length3)
{
    point3_t p = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(0.0f, length3(p));
}

TEST(point, length3_1)
{
    point3_t p = { 1.0f, 0.0f, 0.0f };

    EXPECT_EQ(1.0f, length3(p));
}

TEST(point, length3_2)
{
    point3_t p = { 0.0f, 2.0f, 0.0f };

    EXPECT_EQ(2.0f, length3(p));
}

TEST(point, length_squared3)
{
    point3_t p = { 2.0f, 2.0f, 2.0f };

    EXPECT_EQ(12.0f, length_squared3(p));
}

TEST(point, midpoint3)
{
    point3_t p1 = { -1.0f, 0.0f, 1.0f };
    point3_t p2 = { 1.0f, 0.0f, -1.0f };

    point3_t m;

    midpoint3(p1, p2, m);

    EXPECT_EQ(0.0f, m[0]);
    EXPECT_EQ(0.0f, m[1]);
    EXPECT_EQ(0.0f, m[2]);
}

TEST(point, mul3)
{
    point3_t p1 = { -1.0f, 2.0f, 3.0f };

    mul3(p1, 2.0f);

    EXPECT_EQ(-2.0f, p1[0]);
    EXPECT_EQ(4.0f, p1[1]);
    EXPECT_EQ(6.0f, p1[2]);
}

TEST(point2d, comparisons)
{
    EXPECT_TRUE(pa1<point2d>() == pa2<point2d>());
    EXPECT_FALSE(pa1<point2d>() != pa2<point2d>());
    EXPECT_TRUE(pa1<point2d>() >= pa2<point2d>());
    EXPECT_TRUE(pa1<point2d>() <= pa2<point2d>());
    EXPECT_FALSE(pa1<point2d>() < pa2<point2d>());
    EXPECT_FALSE(pa1<point2d>() > pa2<point2d>());

    EXPECT_FALSE(pa1<point2d>() == pb1<point2d>());
    EXPECT_TRUE(pa1<point2d>() != pb1<point2d>());
    EXPECT_TRUE(pb1<point2d>() >= pa1<point2d>());
    EXPECT_TRUE(pa1<point2d>() <= pb1<point2d>());
    EXPECT_TRUE(pa1<point2d>() < pb1<point2d>());
    EXPECT_TRUE(pb1<point2d>() > pa1<point2d>());
}

TEST(point3d, comparisons)
{
    EXPECT_TRUE(pa1<point3d>() == pa2<point3d>());
    EXPECT_FALSE(pa1<point3d>() != pa2<point3d>());
    EXPECT_TRUE(pa1<point3d>() >= pa2<point3d>());
    EXPECT_TRUE(pa1<point3d>() <= pa2<point3d>());
    EXPECT_FALSE(pa1<point3d>() < pa2<point3d>());
    EXPECT_FALSE(pa1<point3d>() > pa2<point3d>());

    EXPECT_FALSE(pa1<point3d>() == pb1<point3d>());
    EXPECT_TRUE(pa1<point3d>() != pb1<point3d>());
    EXPECT_TRUE(pb1<point3d>() >= pa1<point3d>());
    EXPECT_TRUE(pa1<point3d>() <= pb1<point3d>());
    EXPECT_TRUE(pa1<point3d>() < pb1<point3d>());
    EXPECT_TRUE(pb1<point3d>() > pa1<point3d>());
}

TEST(point42d, comparisons)
{
    EXPECT_TRUE(pa1<point4d>() == pa2<point4d>());
    EXPECT_FALSE(pa1<point4d>() != pa2<point4d>());
    EXPECT_TRUE(pa1<point4d>() >= pa2<point4d>());
    EXPECT_TRUE(pa1<point4d>() <= pa2<point4d>());
    EXPECT_FALSE(pa1<point4d>() < pa2<point4d>());
    EXPECT_FALSE(pa1<point4d>() > pa2<point4d>());

    EXPECT_FALSE(pa1<point4d>() == pb1<point4d>());
    EXPECT_TRUE(pa1<point4d>() != pb1<point4d>());
    EXPECT_TRUE(pb1<point4d>() >= pa1<point4d>());
    EXPECT_TRUE(pa1<point4d>() <= pb1<point4d>());
    EXPECT_TRUE(pa1<point4d>() < pb1<point4d>());
    EXPECT_TRUE(pb1<point4d>() > pa1<point4d>());
}