#include "solosnake/testing/testing.hpp"
#include "solosnake/persistance.hpp"

using namespace solosnake;

TEST(persistance, ctor)
{
    {
        persistance p;
    }
}

TEST(persistance, set)
{
    {
        persistance p;
        p.set("key", "value", std::string());
        p.set("red", "rose", "rose comment");
        EXPECT_TRUE(p["red"] == std::string("rose"));
        EXPECT_TRUE(p["red"].comment() == std::string("rose comment"));
        EXPECT_TRUE(p["key"] == "value");
    }
}

TEST(persistance, equality)
{
    persistance p;
    persistance q;

    EXPECT_TRUE(p == q);

    p.set("key", "value", std::string());
    p.set("red", "rose", std::string());
    p.set("blue", "thunder", std::string());

    EXPECT_TRUE(p != q);
    EXPECT_FALSE(p == q);

    q.set("key", "value", std::string());

    EXPECT_TRUE(p != q);
    EXPECT_FALSE(p == q);

    q.set("red", "rose", std::string());

    EXPECT_TRUE(p != q);
    EXPECT_FALSE(p == q);

    q.set("blue", "thunder", std::string());

    EXPECT_TRUE(p == q);
    EXPECT_FALSE(p != q);
}

TEST(persistance, save_to_xml)
{
    {
        persistance p;
        p.set("key", "value", "value comment");
        p.set("red", "rose", "rose comment");
        p.set("blue", "thunder", "thunder comment");

        EXPECT_TRUE(p.save_to_xml("test_save.xml"));

        persistance q;

        EXPECT_FALSE(p == q);
        EXPECT_TRUE(q.load_from_xml("test_save.xml"));
        EXPECT_TRUE(p == q);
    }
}