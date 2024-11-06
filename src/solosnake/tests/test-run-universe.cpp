#include "solosnake/testing/testing.hpp"
#include "solosnake/evelike-universe.hpp"
#include "solosnake/evelike-run-universe.hpp"

using namespace  solosnake::evelike;

TEST( RunUniverse, Run_debug )
{
    const auto filename = "Run-Universe-Test-32u.json";
    Universe universeA;
    universeA.add_random_solar_systems(32u, 234u);
    universeA.advance_n_frames(100u);
    universeA.save(filename);
    auto debugging = true;
    auto time_limit_ms = 150000;
    auto result = run_universe(filename, time_limit_ms, debugging);
}

TEST( RunUniverse, Run_non_debug )
{
    const auto filename = "Run-Universe-Test-320u.json";
    Universe universeA;
    universeA.add_random_solar_systems(320u, 567u);
    universeA.advance_n_frames(100u);
    universeA.save(filename);
    auto debugging = false;
    auto time_limit_ms = 4500;
    auto result = run_universe(filename, time_limit_ms, debugging);
}

TEST( RunUniverse, Run_max_solar_systems )
{
    const auto filename = "Run-Universe-Test-65335u.json";
    Universe universeA;
    universeA.add_random_solar_systems(65535u, 123u);
    universeA.advance_n_frames(100u);
    universeA.save(filename);
    auto debugging = false;
    auto time_limit_ms = 5000;
    auto result = run_universe(filename, time_limit_ms, debugging);
}

TEST( RunUniverse, Run_too_many_solar_systems )
{
    Universe universeA;
    EXPECT_THROW( universeA.add_random_solar_systems(65536u, 123u) );
}