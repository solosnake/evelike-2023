#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_emissive_channels.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Emissive_channels, equality )
{
    Emissive_channels e1{};
    e1.emissive_readouts[0][1] = 128;

    Emissive_channels e2{};
    e2.emissive_colours[0] = Bgra(8, 16, 32, 64);

    EXPECT_TRUE( e1 == e1 );
    EXPECT_FALSE( e1 != e1 );
    EXPECT_FALSE( e1 == e2 );
    EXPECT_TRUE( e1 != e2 );
}

TEST( Emissive_channels, emissivechannels_to_from_json )
{
    Emissive_channels e;
    e.emissive_readouts[0][1] = 128;
    e.emissive_colours[0] = Bgra(8, 16, 32, 64);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = e;
    auto e2 = j.get<Emissive_channels>();
    EXPECT_EQ( e, e2 );
}
