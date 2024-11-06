#ifndef SOLOSNAKE_EVELIKE_GAME_HPP
#define SOLOSNAKE_EVELIKE_GAME_HPP

#include <cstdint>
#include <utility>
#include "solosnake/screen.hpp"
#include "solosnake/gamegl.hpp"
#include "solosnake/evelike-game-data.hpp"

namespace solosnake::evelike
{
    /// The EveLike_game has no managed pointers to the screens, so the screens
    /// can safely have pointers back to the game without creating any lifetime
    /// cycles.
    class EveLike_game final : public solosnake::GameGL
    {
    public:

        /// Constructs the EveLike game from a universe loaded from the the
        /// given file. The maximum run time can be set by passing in a non-zero
        /// `max_milliseconds` value.
        /// @param universe_file    Path to the universe file to open.
        /// @param debugging        If `true` debugging options will be enabled.
        /// @param time_limit       Game exits after this many milliseconds.
        EveLike_game(const std::string_view& universe,
                     bool debugging,
                     std::uint64_t max_milliseconds);

        static constexpr auto AppName       = "EveLike";
        static constexpr auto OrgName       = "solosnake";
        static constexpr auto OpenGLVersion = std::make_pair(4,6);

        EveLike_game_data*            game_data() noexcept;

        const EveLike_game_data*      game_data() const noexcept;

    private:

        std::shared_ptr<Screen>       get_initial_screen() final;

        std::string                   get_application_name() noexcept final;

        std::string                   get_organisation_name() noexcept final;

        std::pair<int,int>            get_gl_version() noexcept final;

        std::string                   get_window_title() noexcept final;

        std::optional<Image>          get_window_icon_image() noexcept final;

        std::uint64_t                 get_max_runtime_ms() const noexcept final;

    private:

        std::shared_ptr<EveLike_game_data>  m_game_data;
    };

    //--------------------------------------------------------------------------

    inline EveLike_game_data* EveLike_game::game_data() noexcept
    {
        return m_game_data.get();
    }

    inline const EveLike_game_data* EveLike_game::game_data() const noexcept
    {
        return m_game_data.get();
    }

}
#endif
