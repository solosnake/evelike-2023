#ifndef SOLOSNAKE_EVELIKE_SCREEN_LOCATION_HPP
#define SOLOSNAKE_EVELIKE_SCREEN_LOCATION_HPP

#include <memory>
#include "solosnake/evelike-screen.hpp"
#include "solosnake/evelike-game.hpp"
#include "solosnake/opengl.hpp"

namespace solosnake::evelike
{
    class Location_screen final : public Evelike_screen
    {
    public:

        explicit Location_screen(std::shared_ptr<EveLike_game>);

        void on_window_size_change(unsigned int width,
                                   unsigned int height) noexcept final;

        void on_keydown(std::int32_t, std::uint16_t ) noexcept final;

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
