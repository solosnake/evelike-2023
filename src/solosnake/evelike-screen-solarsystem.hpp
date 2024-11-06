#ifndef SOLOSNAKE_EVELIKE_SCREEN_SOLARSYSTEM_HPP
#define SOLOSNAKE_EVELIKE_SCREEN_SOLARSYSTEM_HPP

#include <memory>
#include "solosnake/evelike-screen.hpp"
#include "solosnake/evelike-game.hpp"

namespace solosnake::evelike
{
    class SolarSystem_screen final : public Evelike_screen
    {
    public:

        explicit SolarSystem_screen(std::shared_ptr<EveLike_game>);

        void on_window_size_change(unsigned int width,
                                   unsigned int height) noexcept final;

        void on_keydown(std::int32_t, std::uint16_t ) noexcept final;

        void on_mouse_move(int x, int y) noexcept final;

        void on_mouse_wheel(float horizontal, float vertical) noexcept final;

        void on_mouse_click_down(MouseButton, bool single, int x, int y) noexcept final;

        void on_mouse_click_up(MouseButton, bool single, int x, int y) noexcept final;

        void on_quit() noexcept final;

        /// Called prior to screen render/update usage.
        void prepare() final;

        /// When this returns true the system swaps the back buffer.
        bool render() final;

        Action update( std::uint64_t elapsed_ms ) final;

        std::shared_ptr<Screen> get_next_screen() const noexcept final;

    private:

        OpenGL*                                 m_opengl;
        mutable std::unique_ptr<Screen>         m_next_screen;
        bool                                    m_quit;
    };
}

#endif
