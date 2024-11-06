#include "solosnake/testing/testing.hpp"
#include "solosnake/nlohmann/json.hpp"
#include "solosnake/evelike-json.hpp"
#include "solosnake/evelike-solarsystem.hpp"
#include "solosnake/evelike-universe.hpp"

using namespace solosnake::evelike;

TEST( EveLikeJson, Universe )
{
    Universe universe;
    nlohmann::json j = universe;
    auto universe2 = j.get<Universe>();
    EXPECT_EQ( universe, universe2 );
}

TEST( EveLikeJson, SolarSystem )
{
    SolarSystem ss;
    nlohmann::json j = ss;
    auto ss2 = j.get<SolarSystem>();
    EXPECT_EQ( ss, ss2 );
}