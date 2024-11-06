#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Thruster, wrong_category )
{
    Component c("Test",
                12345,
                1000,
                IndustrialComponent,
                Amount( ChondriteOre, 123 ),
                1.23f);

    std::int32_t power = 120;

    EXPECT_THROW(  Thruster( c, power ) );
}

TEST( Thruster, Thruster )
{
    Component c("Test",
                12345,
                1000,
                PropulsionComponent,
                Amount( ChondriteOre, 123 ),
                1.23f);

    std::int32_t power = 120;

    const Thruster t( c, power );

    EXPECT_TRUE( t == t );
    EXPECT_FALSE( t != t );
    EXPECT_EQ( t.details(), c );
    EXPECT_EQ( t.power(), power );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = t;
    auto t2 = j.get<Thruster>();
    EXPECT_EQ( t, t2 );
}
