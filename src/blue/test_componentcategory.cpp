#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_componentcategory.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( ComponentCategory, to_from_string )
{
    EXPECT_EQ( "IndustrialComponent", blue::to_string(IndustrialComponent) );
    EXPECT_EQ( "DefensiveComponent", blue::to_string(DefensiveComponent) );
    EXPECT_EQ( "OffensiveComponent", blue::to_string(OffensiveComponent) );
    EXPECT_EQ( "PropulsionComponent", blue::to_string(PropulsionComponent) );
    EXPECT_EQ( "DualUseComponent", blue::to_string(DualUseComponent) );

    EXPECT_EQ( DualUseComponent, blue::component_category_from_string("DualUseComponent") );
    EXPECT_EQ( IndustrialComponent, blue::component_category_from_string("IndustrialComponent") );
    EXPECT_EQ( OffensiveComponent, blue::component_category_from_string("OffensiveComponent") );
    EXPECT_EQ( PropulsionComponent, blue::component_category_from_string("PropulsionComponent") );
    EXPECT_EQ( DefensiveComponent, blue::component_category_from_string("DefensiveComponent") );
}

TEST( ComponentCategory, to_from_json )
{
    ComponentCategory categorisation = IndustrialComponent;

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = categorisation;
    auto c2 = j.get<ComponentCategory>();
    EXPECT_EQ( categorisation, c2 );
}
