#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Tradables, to_from_json )
{
    Tradables t( Amount(Ataxite, 5u) );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = t;
    auto t2 = j.get<Tradables>();
    EXPECT_EQ( t, t2 );
}