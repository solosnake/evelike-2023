#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_line2d.hpp"
#include <fstream>

using namespace blue;

namespace blue
{
    constexpr float PI_RADIANS         {3.1415926535897932384626433832795f};
    constexpr float HALF_PI_RADIANS    {0.50f * PI_RADIANS};
    constexpr float QUARTER_PI_RADIANS {0.25f * PI_RADIANS};

    std::ostream& operator<<(std::ostream& os, const Point2d& p)
    {
        os << "x = " << p[0] << " y = " << p[1];
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Line2d& L)
    {
        os << L.point_on_line() << " a = " << L.angle().value();
        return os;
    }
}

TEST(Line2d, two_ctors)
{
    Line2d a(Point2d(1.0f, 2.0f), Point2d(3.0f, 4.0f));
    Line2d b(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(a, b);
}

TEST(Line2d, y_equal_zero)
{
    Line2d line(Point2d(1.0f, 0.0f), Point2d(2.0f, 0.0f));
    EXPECT_FLOAT_EQ(line.angle().value(), 0.0f);
    EXPECT_EQ(Point2d(1.0f, 0.0f), line.point_on_line());
}

TEST(Line2d, x_equal_zero)
{
    Line2d line(Point2d(0.0f, 1.0f), Point2d(0.0f, 2.0f));
    EXPECT_FLOAT_EQ(line.angle().value(), HALF_PI_RADIANS);
    EXPECT_EQ(Point2d(0.0f, 1.0f), line.point_on_line());
}

TEST(Line2d, x_equal_two)
{
    Line2d line(Point2d(2.0f, 1.0f), Point2d(2.0f, 2.0f));
    EXPECT_FLOAT_EQ(line.angle().value(), HALF_PI_RADIANS);
    EXPECT_EQ(Point2d(2.0f, 1.0f), line.point_on_line());
}

TEST(Line2d, x_equal_y)
{
    Line2d line(Point2d(1.0f, 1.0f), Point2d(2.0f, 2.0f));
    EXPECT_FLOAT_EQ(line.angle().value(), QUARTER_PI_RADIANS);
    EXPECT_EQ(Point2d(1.0f, 1.0f), line.point_on_line());
}