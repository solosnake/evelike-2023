#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_good_rand.hpp"


TEST( make_good_rand, Good_rand )
{
    auto r0 = blue::make_good_rand(1234U);
    auto r1 = blue::make_good_rand(1234U);

    EXPECT_TRUE(r0.get());
    EXPECT_TRUE(r1.get());

    for(auto i=0; i <1000; ++i)
    {
        auto d0 = r0->drand();
        auto d1 = r1->drand();
        auto f0 = r0->frand();
        auto f1 = r1->frand();
        auto u0 = r0->urand();
        auto u1 = r1->urand();
        EXPECT_TRUE( d0 == d1 );
        EXPECT_TRUE( f0 == f1 );
        EXPECT_TRUE( u0 == u1 );
        EXPECT_TRUE( d0 >= 0.0 );
        EXPECT_TRUE( d0 <= 1.0);
        EXPECT_TRUE( f0 >= 0.0f );
        EXPECT_TRUE( f0 <= 1.0f);
        EXPECT_TRUE( u0 >= 0 );
        EXPECT_TRUE( u0 <= 0xFFFFFFFF);
    }
}
