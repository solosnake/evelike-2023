#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Softpoint, Softpoint )
{
    Hardware_modifier h;
    h.delta_armour_hitpoints            = 500;
    h.delta_cap_broadcast_cost_per_tile = 200;
    h.factor_max_sensing_radius         = 0.51f;
    h.factor_tiles_sensed_per_10_ticks  = 0.52f;

    const std::string_view name      = "Test Thing";
    std::int32_t hullhitpoints       = 2000;
    std::int32_t mass                = 5000;
    float build_complexity           = 2.5f;
    ComponentCategory categorisation = IndustrialComponent;
    Amount build_cost;
    Component c(name, hullhitpoints, mass, categorisation,
                build_cost, build_complexity);

    Softpoint sp( c, h );

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = sp;
    auto sp2 = j.get<Softpoint>();
    EXPECT_EQ( sp, sp2 );
}