#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Amount, negatives )
{
    const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT> contents{ -1, 0, 1, 2, 3, 4, 5, 6 };
    EXPECT_THROW( Amount foo( contents ) );
}

TEST( Amount, Amount )
{
    const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT> contents{ 5, 0, 1, 2, 3, 4, 5, 6 };
    const Amount a( contents );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = a;
    auto a2 = j.get<Amount>();
    EXPECT_EQ( a, a2 );
}
