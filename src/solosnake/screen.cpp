#include "solosnake/screen.hpp"

namespace solosnake
{
    Screen::~Screen() noexcept
    {
    }

    void Screen::on_keydown(std::int32_t, std::uint16_t) noexcept
    {
    }

    void Screen::on_keyup(std::int32_t, std::uint16_t) noexcept
    {
    }

    void Screen::on_mouse_move(int, int) noexcept
    {
    }

    void Screen::on_mouse_wheel(float, float) noexcept
    {
    }

    void Screen::on_mouse_click_down(MouseButton, bool, int, int) noexcept
    {
    }

    void Screen::on_mouse_click_up(MouseButton, bool, int, int) noexcept
    {
    }

    void Screen::on_window_size_change(unsigned int, unsigned int) noexcept
    {
    }

    void Screen::on_quit() noexcept
    {
    }

    void Screen::prepare(std::shared_ptr<GameGL>)
    {
    }

    std::shared_ptr<Screen> Screen::get_next_screen() const noexcept
    {
        return std::shared_ptr<Screen>();
    }

    Screen::Action Screen::update( std::uint64_t )
    {
        return Action::Quit;
    }

    bool Screen::render()
    {
        return true;
    }
}

