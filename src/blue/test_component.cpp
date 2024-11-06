#include <string>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_component.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Component, Component )
{
    const std::string_view name = "Test Thing";
    std::int32_t hull_hitpoints = 2000;
    std::int32_t mass = 5000;
    ComponentCategory categorisation = IndustrialComponent;
    Amount build_cost;
    float build_complexity = 2.5f;

    Component c(name, hull_hitpoints, mass, categorisation,
                build_cost, build_complexity);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = c;
    auto c2 = j.get<Component>();
    EXPECT_EQ( c, c2 );
}
