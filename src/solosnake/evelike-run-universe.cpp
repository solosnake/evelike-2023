#include <memory>
#include "solosnake/evelike-run-universe.hpp"
#include "solosnake/evelike-game.hpp"

namespace solosnake::evelike
{
    int run_universe(const std::string_view& universe_file,
                     const std::uint64_t max_milliseconds,
                     const bool debugging)
    {
        return solosnake::GameGL::run_game<EveLike_game>(universe_file,
                                                         debugging,
                                                         max_milliseconds);
    }
}
