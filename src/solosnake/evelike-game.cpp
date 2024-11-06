
#include <chrono>
#include <memory>
#include <thread>
#include <string>
#include "SDL.h"
#include "SDL_opengl.h"
#include "solosnake/evelike-icon.hpp"
#include "solosnake/evelike-game.hpp"
#include "solosnake/evelike-screen-loading.hpp"

namespace solosnake::evelike
{
    std::pair<int,int> EveLike_game::get_gl_version() noexcept
    {
        return EveLike_game::OpenGLVersion;
    }

    std::string EveLike_game::get_organisation_name() noexcept
    {
        return EveLike_game::OrgName;
    }

    std::string EveLike_game::get_application_name() noexcept
    {
        return EveLike_game::AppName;
    }

    std::string EveLike_game::get_window_title() noexcept
    {
        return get_application_name();
    }

    std::optional<Image> EveLike_game::get_window_icon_image() noexcept
    {
        return evelike::get_evelike_window_icon_image();
    }

    std::uint64_t EveLike_game::get_max_runtime_ms() const noexcept
    {
        return m_game_data->max_milliseconds;
    }

    EveLike_game::EveLike_game(const std::string_view& universe_file,
                               const bool debugging,
                               const std::uint64_t max_milliseconds )
        : m_game_data( std::make_shared<EveLike_game_data>() )
    {
        m_game_data->universe_file    = universe_file;
        m_game_data->max_milliseconds = max_milliseconds;
        m_game_data->debugging        = debugging;
    }

    std::shared_ptr<Screen> EveLike_game::get_initial_screen()
    {
        auto g = get_dynamic_shared_ptr<EveLike_game>();
        return std::make_shared<Loading_screen>( g );
    }
}
