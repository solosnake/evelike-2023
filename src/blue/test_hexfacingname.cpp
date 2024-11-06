#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hexfacingname.hpp"

TEST( HexFacingName, addition )
{
    blue::HexFacingName x = blue::FacingTile0;
    EXPECT_EQ( x + 0, x );

    EXPECT_EQ( blue::FacingTile1 +  6, blue::FacingTile1 );
    EXPECT_EQ( blue::FacingTile1 + 12, blue::FacingTile1 );
    EXPECT_EQ( blue::FacingTile1 +  5, blue::FacingTile0 );
    EXPECT_EQ( blue::FacingTile2 +  4, blue::FacingTile0 );
    EXPECT_EQ( blue::FacingTile3 +  1, blue::FacingTile4 );
}
