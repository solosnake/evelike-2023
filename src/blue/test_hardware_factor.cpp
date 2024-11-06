#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardware_factor.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Hardware_factor, Hardware_factor )
{
    Hardware_factor h1(0.625f);
    Hardware_factor h2(0.925f);

    EXPECT_TRUE( h1 == h1 );
    EXPECT_TRUE( h2 == h2 );
    EXPECT_FALSE( h1 != h1 );

    EXPECT_TRUE( h1 != h2 );
    EXPECT_FALSE( h1 == h2 );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = h1;
    auto h3 = j.get<Hardware_factor>();
    EXPECT_EQ( h1, h3 );
}
