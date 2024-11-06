#include <array>
#include <cassert>
#include <filesystem>
#include "SDL.h"
#include "solosnake/opengl.hpp"
#include "solosnake/gamegl.hpp"
#include "solosnake/evelike-screen-failure.hpp"

namespace solosnake::evelike
{
    Failure_screen::Failure_screen(std::shared_ptr<EveLike_game> game)
        : Evelike_screen(game)
        , m_quit(false)
    {
        assert( game );
    }

    void Failure_screen::on_window_size_change(const unsigned int width,
                                               const unsigned int height) noexcept
    {
        resize_viewport(width, height);
    }

    void Failure_screen::prepare()
    {
        resize_viewport();
        gl()->EnableDepthTest(true);
        gl()->EnableBackFaceCulling(false);
        gl()->EnableLinePolygonMode(true);
        gl()->ClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        game_data()->view_gl_state.view_projection.update();
    }

    void Failure_screen::on_keydown(const std::int32_t key,
                                    const std::uint16_t ) noexcept
    {
        switch(key)
        {
        case SDLK_q:
            m_quit = true;
            break;
        }
    }

    void Failure_screen::on_quit() noexcept
    {
        m_quit = true;
    }

    Screen::Action Failure_screen::update( std::uint64_t )
    {
        game_data()->view_gl_state.view_projection.update();
        auto a = m_next_screen ? Screen::Action::ChangeScreen : Screen::Action::Continue;
        return m_quit ? Screen::Action::Quit : a;
    }

    std::shared_ptr<Screen> Failure_screen::get_next_screen() const noexcept
    {
        return std::shared_ptr<Screen>(m_next_screen.release());
    }

    bool Failure_screen::render()
    {
        gl()->Clear(OpenGL::COLOR_BUFFER_BIT | OpenGL::DEPTH_BUFFER_BIT);
        return true;
    }
}
