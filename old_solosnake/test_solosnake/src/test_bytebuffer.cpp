#include "solosnake/testing/testing.hpp"
#include "solosnake/bytebuffer.hpp"

using namespace solosnake::network;

TEST(bytebuffer, ctor)
{
    {
        bytebuffer b;

        EXPECT_TRUE(b.is_empty());
        EXPECT_TRUE(b.data() == 0);
        EXPECT_TRUE(b.size() == 0);
    }
}

TEST(bytebuffer, lt1)
{
    {
        bytebuffer a;
        bytebuffer b;
        EXPECT_FALSE(a < b);
        EXPECT_FALSE(b < a);
    }
}

TEST(bytebuffer, lt2)
{
    {
        bytebuffer a("abc");
        bytebuffer b("def");
        EXPECT_TRUE(a < b);
        EXPECT_FALSE(b < a);
    }
}

TEST(bytebuffer, eq)
{
    {
        bytebuffer a;
        bytebuffer b;
        EXPECT_TRUE(a == b);

        bytebuffer c("hello world");
        bytebuffer d("hello world");
        EXPECT_TRUE(c == d);
    }
}