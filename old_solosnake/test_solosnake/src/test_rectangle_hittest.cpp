#include "solosnake/testing/testing.hpp"
#include "solosnake/rectangle_hittest.hpp"

using namespace solosnake;

TEST(rectangle_hittest, corners)
{
    {
        rectangle_hittest<int> r;

        r.add_rect(rect(0, 0, dimension2dui(10, 10)), 0);

        std::vector<int> under;

        r.get_rects_under(0, 0, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(0, 9, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(0, 10, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(9, 0, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(10, 0, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(9, 9, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(10, 10, under);
        EXPECT_TRUE(under.size() == 0);
    }
}

TEST(rectangle_hittest, sides)
{
    {
        rectangle_hittest<int> r;

        r.add_rect(rect(0, 0, dimension2dui(10, 10)), 0);

        std::vector<int> under;

        r.get_rects_under(0, 5, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(5, 0, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(9, 5, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(5, 9, under);
        EXPECT_TRUE(under.size() == 1);
    }
}

TEST(rectangle_hittest, change_rect)
{
    {
        rectangle_hittest<int> r;

        r.add_rect(rect(0, 0, dimension2dui(10, 10)), 0);

        std::vector<int> under;

        r.get_rects_under(0, 0, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(0, 9, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(9, 0, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(9, 9, under);
        EXPECT_TRUE(under.size() == 1);

        r.get_rects_under(25, 25, under);
        EXPECT_TRUE(under.empty());

        r.update_rect(0, rect(20, 20, dimension2dui(10, 10)));

        r.get_rects_under(0, 0, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(0, 9, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(9, 0, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(9, 9, under);
        EXPECT_TRUE(under.size() == 0);

        r.get_rects_under(25, 25, under);
        EXPECT_FALSE(under.empty());
    }
}