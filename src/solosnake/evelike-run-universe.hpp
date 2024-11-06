#ifndef SOLOSNAKE_EVELIKE_RUN_UNIVERSE_HPP
#define SOLOSNAKE_EVELIKE_RUN_UNIVERSE_HPP

#include <cstdint>
#include <string_view>

namespace solosnake::evelike
{
    int run_universe(const std::string_view& universe_file,
                     std::uint64_t max_runtime_ms,
                     bool debugging);
}

#endif