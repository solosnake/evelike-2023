#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hardpointtype.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( HardpointType, HardpointType_to_string )
{
    EXPECT_EQ( "WeaponHardpoint",   blue::to_string(WeaponHardpoint) );
    EXPECT_EQ( "CapDrainHardpoint", blue::to_string(CapDrainHardpoint) );
    EXPECT_EQ( "CapXFerHardpoint",  blue::to_string(CapXFerHardpoint) );
    EXPECT_EQ( "MinerHardpoint",    blue::to_string(MinerHardpoint) );
    EXPECT_EQ( "ArmourRepairerHardpoint", blue::to_string(ArmourRepairerHardpoint) );
}

TEST( HardpointType, HardpointType_from_string )
{
    EXPECT_EQ( WeaponHardpoint,   blue::hardpoint_type_from_string("WeaponHardpoint") );
    EXPECT_EQ( CapDrainHardpoint, blue::hardpoint_type_from_string("CapDrainHardpoint") );
    EXPECT_EQ( CapXFerHardpoint,  blue::hardpoint_type_from_string("CapXFerHardpoint") );
    EXPECT_EQ( MinerHardpoint,    blue::hardpoint_type_from_string("MinerHardpoint") );
    EXPECT_EQ( ArmourRepairerHardpoint, blue::hardpoint_type_from_string("ArmourRepairerHardpoint") );
}
