#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include "solosnake/evelike-screen.hpp"

namespace solosnake::evelike
{
    namespace
    {
        /// Returns a file friendly string with the current time and date.
        std::string time_stamp()
        {
            auto now = std::chrono::system_clock::now();
            auto ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            std::stringstream ss;
            ss << ms;
            return ss.str();
        }
    }

    Evelike_screen::Evelike_screen(std::shared_ptr<EveLike_game> game)
        : m_game(game)
        , m_assets_dir(std::filesystem::path(game->get_application_data_directory()) / "assets")
        , m_screenshot_dir(std::filesystem::path(game->get_application_pref_directory()) / "screenshots")
    {
        assert(game);
    }

    Evelike_screen::~Evelike_screen()
    {
    }

    void Evelike_screen::toggle_fullscreen()
    {
        m_game->toggle_fullscreen();
    }

    void Evelike_screen::take_screenshot() noexcept
    {
        try
        {
            if (not std::filesystem::exists(m_screenshot_dir))
            {
                std::filesystem::create_directories(m_screenshot_dir);
            }

            auto file_name = (m_screenshot_dir / ("Screenshot_" + time_stamp() + ".tga")).string();
            gl()->SaveTGAScreenshot(file_name.c_str());
            log_info("Screenshot saved as '" + file_name + "'.");
        }
        catch (const std::exception &e)
        {
            m_game->log_error(std::string("Screenshot failed: ") + e.what());
        }
    }

    std::shared_ptr<EveLike_game> Evelike_screen::get_game_ptr() noexcept
    {
        return m_game;
    }

    void Evelike_screen::prepare(std::shared_ptr<GameGL>)
    {
        this->prepare();
    }

    void Evelike_screen::resize_viewport()
    {
        resize_viewport(game()->get_window_width(),
                        game()->get_window_height());
    }

    void Evelike_screen::resize_viewport(const unsigned int width,
                                         const unsigned int height) noexcept
    {
        gl()->SetViewport(0, 0, width, height);
        game_data()->view_gl_state.mouse_ball.set_window_size(width, height);
        game_data()->view_gl_state.view_projection.set_viewport(width, height);
    }

    std::string Evelike_screen::shader_file_path(const std::string_view& file) const noexcept
    {
        return (m_assets_dir / "shaders" / file).string();
    }
}