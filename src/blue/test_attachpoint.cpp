#include <string>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_attachpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( AttachPoint, bad_attachpoints )
{
    EXPECT_THROW( AttachPoint(0) );
    EXPECT_THROW( AttachPoint(9) );
    EXPECT_THROW( AttachPoint(19) );
    EXPECT_THROW( AttachPoint(29) );
    EXPECT_THROW( AttachPoint(39) );
    EXPECT_THROW( AttachPoint(49) );
    EXPECT_THROW( AttachPoint(59) );
    EXPECT_THROW( AttachPoint(69) );
    EXPECT_THROW( AttachPoint(79) );
    EXPECT_THROW( AttachPoint(100) );
}

TEST( AttachPoint, AttachPoint )
{
    AttachPoint ap(81);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = ap;
    auto ap2 = j.get<AttachPoint>();
    EXPECT_EQ( ap, ap2 );
}

TEST( AttachPoint, order )
{
    AttachPoint a( 22 );
    AttachPoint b( 27 );

    EXPECT_FALSE( a < a );
    EXPECT_FALSE( b < a );
    EXPECT_TRUE( a < b );
}
