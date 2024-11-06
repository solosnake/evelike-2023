#include <array>
#include <cassert>
#include <filesystem>
#include "SDL.h"
#include "solosnake/opengl.hpp"
#include "solosnake/gamegl.hpp"
#include "solosnake/evelike-screen-solarsystem.hpp"

namespace solosnake::evelike
{
    SolarSystem_screen::SolarSystem_screen(std::shared_ptr<EveLike_game> game)
        : Evelike_screen(game)
        , m_opengl(game->opengl())
        , m_quit(false)
    {
        assert( game );
        assert( m_opengl );
    }

    void SolarSystem_screen::on_window_size_change(const unsigned int width,
                                                   const unsigned int height) noexcept
    {
        resize_viewport(width, height);
    }

    void SolarSystem_screen::prepare()
    {
        assert(m_opengl);
        resize_viewport();
        m_opengl->EnableDepthTest(true);
        m_opengl->EnableBackFaceCulling(false);
        m_opengl->EnableLinePolygonMode(true);
        m_opengl->ClearColor(0.0f, 0.0f, 0.9f, 1.0f);
        game_data()->view_gl_state.view_projection.update();
    }

    void SolarSystem_screen::on_keydown(const std::int32_t key,
                                        const std::uint16_t ) noexcept
    {
        switch(key)
        {
        case SDLK_q:
            m_quit = true;
            break;

        case SDLK_UP:
            break;

        case SDLK_DOWN:
            break;

        case SDLK_F11:
            toggle_fullscreen();
            break;

        case SDLK_F12:
            take_screenshot();
            break;

        default:
            break;
        }
    }

    void SolarSystem_screen::on_mouse_move(const int , const int ) noexcept
    {
    }

    void SolarSystem_screen::on_mouse_wheel(const float ,
                                            const float ) noexcept
    {
    }

    void SolarSystem_screen::on_mouse_click_down(const MouseButton,
                                                 const bool ,
                                                 const int ,
                                                 const int ) noexcept
    {
    }

    void SolarSystem_screen::on_mouse_click_up(const MouseButton,
                                               const bool ,
                                               const int ,
                                               const int ) noexcept
    {
    }

    void SolarSystem_screen::on_quit() noexcept
    {
        m_quit = true;
    }

    Screen::Action SolarSystem_screen::update( std::uint64_t )
    {
        game_data()->view_gl_state.view_projection.update();
        auto a = m_next_screen ? Screen::Action::ChangeScreen : Screen::Action::Continue;
        return m_quit ? Screen::Action::Quit : a;
    }

    std::shared_ptr<Screen> SolarSystem_screen::get_next_screen() const noexcept
    {
        return std::shared_ptr<Screen>(m_next_screen.release());
    }

    bool SolarSystem_screen::render()
    {
        m_opengl->Clear( OpenGL::COLOR_BUFFER_BIT | OpenGL::DEPTH_BUFFER_BIT );
        return true;
    }
}
