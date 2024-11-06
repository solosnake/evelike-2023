#include "solosnake/testing/testing.hpp"
#include <utility>
#include "solosnake/bestfit.hpp"
#include "solosnake/image.hpp"
#include "solosnake/colour.hpp"

using namespace solosnake;

namespace
{
std::pair<unsigned int, unsigned int> dimensions(unsigned int a, unsigned int b)
{
    return std::make_pair(a > b ? a : b, a > b ? b : a);
}

std::pair<unsigned int, unsigned int> dimensions(const bestfit::rectangle& r)
{
    return dimensions(r.width(), r.height());
}
}

TEST(bounding_rectangle, bounding_rectangle)
{
    std::vector<bestfit::rectangle> problemset;
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));

    std::vector<bestfit::placedrect> solution;

    solution.push_back(bestfit::placedrect(0, problemset[0], 0, 0, false));
    solution.push_back(bestfit::placedrect(1, problemset[1], 10, 0, false));
    solution.push_back(bestfit::placedrect(2, problemset[2], 0, 10, false));
    solution.push_back(bestfit::placedrect(3, problemset[3], 10, 10, false));

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(20, bestrect.width());
    EXPECT_EQ(20, bestrect.height());
}

TEST(bounding_rectangle, bounding_rectangle_2)
{
    std::vector<bestfit::rectangle> problemset;
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));

    std::vector<bestfit::placedrect> solution;

    solution.push_back(bestfit::placedrect(0, problemset[0], 0, 0, false));
    solution.push_back(bestfit::placedrect(1, problemset[1], 10, 10, false));

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(20, bestrect.width());
    EXPECT_EQ(20, bestrect.height());
}

TEST(bestfitrectangle, no_rectangles)
{
    std::vector<bestfit::rectangle> problemset;
    EXPECT_TRUE(best_fit_rectangle_containing(problemset).empty());
}

TEST(bestfitrectangle, one_rectangles)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(100, 200));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 1);

    save_bestfit_as_bitmap("one_rectangles.bmp", solution);
}

TEST(bestfitrectangle, simple_4_square)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));
    problemset.push_back(bestfit::rectangle(10, 10));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 4);

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(20 * 20, bestrect.width() * bestrect.height());

    save_bestfit_as_bitmap("simple_4_square.bmp", solution);
}

TEST(bestfitrectangle, three1x1_one1x3)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(8, 8));
    problemset.push_back(bestfit::rectangle(4, 4));
    problemset.push_back(bestfit::rectangle(2, 2));
    problemset.push_back(bestfit::rectangle(1, 1));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 4);

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(12, dimensions(bestrect).first);
    EXPECT_EQ(8, dimensions(bestrect).second);

    save_bestfit_as_bitmap("three1x1_one1x3.bmp", solution);
}

TEST(bestfitrectangle, problemset_1)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(4, 8));
    problemset.push_back(bestfit::rectangle(6, 6));
    problemset.push_back(bestfit::rectangle(3, 5));
    problemset.push_back(bestfit::rectangle(5, 3));
    problemset.push_back(bestfit::rectangle(2, 3));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 5);

    auto bestrect = bounding_rectangle(solution);

    // EXPECT_EQ( 12, dimensions(bestrect).first );
    // EXPECT_EQ(  8, dimensions(bestrect).second );

    save_bestfit_as_bitmap("problemset_1.bmp", solution);
}

/**
 * Can be fit together into a 5 x 5 rectangle.
 */
TEST(bestfitrectangle, problemset_2)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(2, 3));
    problemset.push_back(bestfit::rectangle(2, 2));
    problemset.push_back(bestfit::rectangle(1, 3));
    problemset.push_back(bestfit::rectangle(1, 1));
    problemset.push_back(bestfit::rectangle(1, 2));
    problemset.push_back(bestfit::rectangle(1, 2));
    problemset.push_back(bestfit::rectangle(1, 1));
    problemset.push_back(bestfit::rectangle(3, 2));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 8);

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(5, dimensions(bestrect).first);
    EXPECT_EQ(5, dimensions(bestrect).second);

    save_bestfit_as_bitmap("problemset_2.bmp", solution);
}

/**
 * Can be fit together into a 5 x 5 rectangle.
 */
TEST(bestfitrectangle, problemset_2_reversed)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(3, 2));
    problemset.push_back(bestfit::rectangle(2, 2));
    problemset.push_back(bestfit::rectangle(3, 1));
    problemset.push_back(bestfit::rectangle(1, 1));
    problemset.push_back(bestfit::rectangle(2, 1));
    problemset.push_back(bestfit::rectangle(2, 1));
    problemset.push_back(bestfit::rectangle(1, 1));
    problemset.push_back(bestfit::rectangle(2, 3));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_TRUE(solution.size() == 8);

    auto bestrect = bounding_rectangle(solution);

    EXPECT_EQ(5, dimensions(bestrect).first);
    EXPECT_EQ(5, dimensions(bestrect).second);

    save_bestfit_as_bitmap("problemset_2_reversed.bmp", solution);
}

/**
 * A 10 x 8 rectangle with no clear "longest" edge, and a hole.
 *
 * 0001111111
 * 0001111111
 * 0001111111
 * 0002  4444
 * 0002554444
 * 3333334444
 * 3333334444
 * 3333334444
 */
TEST(bestfitrectangle, problemset_4)
{
    std::vector<bestfit::rectangle> problemset;

    problemset.push_back(bestfit::rectangle(3, 5));
    problemset.push_back(bestfit::rectangle(7, 3));
    problemset.push_back(bestfit::rectangle(1, 2));
    problemset.push_back(bestfit::rectangle(2, 1));
    problemset.push_back(bestfit::rectangle(4, 5));
    problemset.push_back(bestfit::rectangle(6, 3));

    auto solution = best_fit_rectangle_containing(problemset);

    EXPECT_EQ(solution.size(), 6);

    auto bestrect = bounding_rectangle(solution);

    save_bestfit_as_bitmap("problemset_4.bmp", solution);
}
