#include "solosnake/testing/testing.hpp"
#include "solosnake/args.hpp"

TEST(args, ctor)
{
    {
        solosnake::args a;

        EXPECT_TRUE(a.argc() > 0);
    }
}