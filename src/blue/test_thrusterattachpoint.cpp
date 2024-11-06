#include <string>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_thruster_attachpoint.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Thruster_attachpoint, bad_thrusterattachpoint_0 )
{
    EXPECT_THROW( Thruster_attachpoint(0) );
}

TEST( Thruster_attachpoint, bad_thrusterattachpoint_80 )
{
    EXPECT_THROW( Thruster_attachpoint(80) );
}

TEST( Thruster_attachpoint, bad_thrusterattachpoint_100 )
{
    EXPECT_THROW( Thruster_attachpoint(100) );
}

TEST( Thruster_attachpoint, Thruster_attachpoint )
{
    Thruster_attachpoint tap(81);

    // Serialise to/from JSON and check the objects are equal.
    nlohmann::json j = tap;
    auto tap2 = j.get<Thruster_attachpoint>();
    EXPECT_EQ( tap, tap2 );
}

TEST( Thruster_attachpoint, order )
{
    Thruster_attachpoint a( 82 );
    Thruster_attachpoint b( 84 );

    EXPECT_FALSE( a < a );
    EXPECT_FALSE( b < a );
    EXPECT_TRUE( a < b );
}

