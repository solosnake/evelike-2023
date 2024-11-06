#include "solosnake/testing/testing.hpp"
#include "solosnake/evelike-universe.hpp"

using namespace solosnake::evelike;

TEST( EveLikeUniverse, LoadSaveAdvance )
{
    const auto filename = "Universe-Test.json";
    Universe universeA;
    universeA.add_random_solar_systems( 10, 1234u );
    universeA.advance_n_frames(100u);
    universeA.save(filename);
    Universe universeB(filename);
    EXPECT_TRUE( universeA == universeB );
}

TEST( EveLikeUniverse, Seeds )
{
    Universe universeA, universeB;
    universeA.add_random_solar_systems( 1000u, 678u );
    universeB.add_random_solar_systems( 1000u, 678u );
    EXPECT_TRUE( universeA == universeB );
}