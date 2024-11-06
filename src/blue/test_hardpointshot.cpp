#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Hardpoint_shot, Hardpoint_shot )
{
    Hardpoint_shot h;
    h.hardpoint_index_ = 4u;
    h.hardpoint_type_ = ArmourRepairerHardpoint;
    h.max_range_ = 20u;
    h.shot_effect_.repairs.armour_repaired = 5u;

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = h;
    auto h2 = j.get<Hardpoint_shot>();
    EXPECT_EQ( h, h2 );
}
