#ifndef SOLOSNAKE_GAME_DATA_HPP
#define SOLOSNAKE_GAME_DATA_HPP

#include <cstdint>
#include <memory>
#include <string>
#include "solosnake/maths.hpp"
#include "solosnake/evelike-universe.hpp"
#include "solosnake/evelike-view-gl-state.hpp"


namespace solosnake::evelike
{
    /// Struct with the data shared amongst the screens of the EveLike game.
    /// This data must not have any managed pointers back to the game or
    /// screens. This must be game state data.
    struct EveLike_game_data
    {
        View_gl_state                   view_gl_state;
        std::shared_ptr<Universe>       universe;
        std::string                     universe_file;
        std::uint64_t                   max_milliseconds{0u};
        bool                            debugging{false};
    };


}

#endif
