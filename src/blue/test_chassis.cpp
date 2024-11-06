#include <string>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_emissive_channels.hpp"
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Chassis, Chassis )
{
    Emissive_channels e;
    e.emissive_readouts[0][1] = 128;
    e.emissive_colours[0] = Bgra(8, 16, 32, 64);

    std::string name("My Big Chassis");
    const Chassis c(name, e);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = c;
    auto c2 = j.get<Chassis>();
    EXPECT_EQ( c, c2 );
}
