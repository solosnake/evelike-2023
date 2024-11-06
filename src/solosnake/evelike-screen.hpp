#ifndef SOLOSNAKE_EVELIKE_SCREEN_HPP
#define SOLOSNAKE_EVELIKE_SCREEN_HPP

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include "solosnake/screen.hpp"
#include "solosnake/evelike-game.hpp"

namespace solosnake::evelike
{
    class Evelike_screen : public solosnake::Screen
    {
    public:

        explicit Evelike_screen(std::shared_ptr<EveLike_game>);

        virtual ~Evelike_screen();

    protected:

        virtual void prepare() = 0;

        void toggle_fullscreen();

        void take_screenshot() noexcept;

        const OpenGL* gl() const noexcept;

        OpenGL* gl() noexcept;

        EveLike_game* game() noexcept;

        const EveLike_game* game() const noexcept;

        std::shared_ptr<EveLike_game> get_game_ptr() noexcept;

        EveLike_game_data* game_data() noexcept;

        const EveLike_game_data* game_data() const noexcept;

        void log_info(const std::string_view& text) noexcept;

        void log_error(const std::string_view& text) noexcept;

        const std::filesystem::path& assets_dir() const noexcept;

        const std::filesystem::path& screenshots_dir() const noexcept;

        std::string shader_file_path(const std::string_view& file) const noexcept;

        void resize_viewport();

        /// Resizes the OpenGL viewport, the view-projection viewport, and the
        /// mouse-ball viewport.
        void resize_viewport(unsigned int width, unsigned int height) noexcept;

    private:

        void prepare(std::shared_ptr<GameGL>) final;

    private:

        std::shared_ptr<EveLike_game>       m_game;
        std::filesystem::path               m_assets_dir;
        std::filesystem::path               m_screenshot_dir;
    };

//------------------------------------------------------------------------------

inline EveLike_game* Evelike_screen::game() noexcept
{
    return m_game.get();
}

inline const EveLike_game* Evelike_screen::game() const noexcept
{
    return m_game.get();
}

inline EveLike_game_data* Evelike_screen::game_data() noexcept
{
    return m_game->game_data();
}

inline const EveLike_game_data* Evelike_screen::game_data() const noexcept
{
    return m_game->game_data();
}

inline void Evelike_screen::log_info(const std::string_view& text) noexcept
{
    m_game->log_info(text);
}

inline void Evelike_screen::log_error(const std::string_view& text) noexcept
{
    m_game->log_error(text);
}

inline const std::filesystem::path& Evelike_screen::assets_dir() const noexcept
{
    return m_assets_dir;
}

inline const std::filesystem::path& Evelike_screen::screenshots_dir() const noexcept
{
    return m_screenshot_dir;
}

inline const OpenGL* Evelike_screen::gl() const noexcept
{
    return m_game->opengl();
}

inline OpenGL* Evelike_screen::gl() noexcept
{
    return m_game->opengl();
}

}
#endif
