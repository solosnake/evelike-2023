#include <string>
#include <vector>
#include "solosnake/testing/testing.hpp"
#include "solosnake/evelike-main.hpp"
#include "solosnake/evelike-universe.hpp"

using namespace solosnake::evelike;

TEST( EveLikeMain, Help )
{
    std::vector<std::string> strings;
    strings.push_back( "EveLike" );
    strings.push_back( "--help" );
    std::vector<char*> argv;
    argv.push_back( strings[0].data() );
    argv.push_back( strings[1].data() );
    auto result = solosnake::evelike::evelike_main(2, argv.data());
    EXPECT_TRUE( result == EXIT_SUCCESS );
}

TEST( EveLikeMain, UniverseFile )
{
    const auto filename = "UniverseFile-EveLikeMain-Test.json";
    Universe universe;
    universe.save(filename);

    std::vector<std::string> strings;
    strings.push_back( "EveLike" );
    strings.push_back( filename );
    std::vector<char*> argv;
    argv.push_back( strings[0].data() );
    argv.push_back( strings[1].data() );
    auto result = solosnake::evelike::evelike_main(2, argv.data());
}
