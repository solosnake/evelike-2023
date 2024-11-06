#include <cmath>
#include <cstdlib>
#include "solosnake/testing/testing.hpp"
#include "solosnake/linearregression3d.hpp"

using namespace solosnake;
using namespace solosnake::linear_regression;

namespace
{
bool approx_eq(double a, double b)
{
    return std::abs(a - b) < 0.05;
}
}

TEST(linearregression3d, empty_vector)
{
    line3d answer;
    std::vector<point3d> empty;
    EXPECT_FALSE(try_get_linear_regression(empty, answer));
}

TEST(linearregression3d, one_point)
{
    line3d answer;
    std::vector<point3d> points;
    points.push_back(point3d(0.0, 0.0, 0.0));
    EXPECT_FALSE(try_get_linear_regression(points, answer));
}

TEST(linearregression3d, worked_example)
{
    line3d answer = {};

    std::vector<point3d> points;

    points.push_back(point3d(33.44, 12.63, 0.314)); // 1
    points.push_back(point3d(28.58, 10.23, 2.729)); // 2
    points.push_back(point3d(30.74, 11.37, 1.618)); // 3
    points.push_back(point3d(32.35, 12.09, 0.820)); // 4
    points.push_back(point3d(17.09, 4.55, 8.409));  // 5
    points.push_back(point3d(15.63, 3.88, 9.173));  // 6
    points.push_back(point3d(27.48, 9.79, 3.279));  // 7
    points.push_back(point3d(19.60, 5.75, 7.180));  // 8
    points.push_back(point3d(27.40, 9.65, 3.295));  // 9
    points.push_back(point3d(17.44, 4.74, 8.257));  // 10

    EXPECT_TRUE(try_get_linear_regression(points, answer));

    EXPECT_TRUE(approx_eq(answer.Xm, 24.975));
    EXPECT_TRUE(approx_eq(answer.Ym, 8.468));
    EXPECT_TRUE(approx_eq(answer.Zm, 4.5074));

    EXPECT_TRUE(approx_eq(answer.Xh, 6.93663));
    EXPECT_TRUE(approx_eq(answer.Yh, -0.459842));
    EXPECT_TRUE(approx_eq(answer.Zh, 13.4831));
}
