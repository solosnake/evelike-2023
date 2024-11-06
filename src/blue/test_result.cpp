#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_result.hpp"

using namespace blue;

TEST( Result, Result )
{
    EXPECT_FALSE( Result() );
    EXPECT_FALSE( Result(false) );
    EXPECT_FALSE( Result("SOMETHING BAD") );
    EXPECT_TRUE( Result(true) );
}

TEST( Result, msg )
{
    Result result("SOMETHING BAD");
    EXPECT_FALSE( result );
}