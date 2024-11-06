#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_componentorientation.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( ComponentOrientation, component_orientation_from_string )
{
    EXPECT_EQ( blue::to_string(Rotated0),   "Rotated0" );
    EXPECT_EQ( blue::to_string(Rotated90),  "Rotated90" );
    EXPECT_EQ( blue::to_string(Rotated180), "Rotated180" );
    EXPECT_EQ( blue::to_string(Rotated270), "Rotated270" );

    EXPECT_EQ( Rotated0,   blue::component_orientation_from_string("Rotated0") );
    EXPECT_EQ( Rotated90,  blue::component_orientation_from_string("Rotated90") );
    EXPECT_EQ( Rotated180, blue::component_orientation_from_string("Rotated180") );
    EXPECT_EQ( Rotated270, blue::component_orientation_from_string("Rotated270") );
}

TEST( ComponentOrientation, to_from_json )
{
    auto c = Rotated180;

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = c;
    auto c2 = j.get<ComponentOrientation>();
    EXPECT_EQ( c, c2 );
}