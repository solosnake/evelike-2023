#ifndef SOLOSNAKE_EVELIKE_GAME_SCREEN_HPP
#define SOLOSNAKE_EVELIKE_GAME_SCREEN_HPP

#include <cstdint>
#include <string_view>
#include <memory>

namespace solosnake
{
    class GameGL;

    /// An abstract state of the game state machine.
    class Screen
    {
    public:

        /// Indicate what action the update loop should take.
        enum class Action
        {
            Quit, Continue, ChangeScreen
        };

        // Values of the mouse button names.
        enum class MouseButton : int
        {
            Left, Middle, Right
        };

        /// Default destructor.
        virtual ~Screen() noexcept;

        /// Called when a user presses a key.
        /// @param key  The identified for key. Matches the SDL_Keycode values and type.
        /// @param modifiers  The modification flags for the key. Matches the
        ///                   SDL_Keymod mask values.
        virtual void on_keydown(std::int32_t key,
                                std::uint16_t modifiers) noexcept;

        /// Called when a user releases a pressed key.
        /// @param key  The identified for key. Matches the SDL_Keycode values and type.
        /// @param modifiers  The modification flags for the key. Matches the
        ///                   SDL_Keymod mask values.
        virtual void on_keyup(std::int32_t key,
                              std::uint16_t modifiers) noexcept;

        /// Called when the mouse moves inside the window. Movements outside
        /// of the window are not reported. (0,0) is at the window top-left.
        /// @param x    X coordinate within the window.
        /// @param y    Y coordinate within the window.
        virtual void on_mouse_move(int x, int y) noexcept;

        /// Called when the mouse wheel is moved horizontally or vertically.
        /// @param horizontal   Positive means to the right and negative to the left.
        /// @param vertical     Positive away from the user and negative toward the user.
        virtual void on_mouse_wheel(float horizontal, float vertical) noexcept;

        /// Called when the mouse is (single or double) clicked, but not yet released.
        /// (0,0) is at the window top-left.
        /// @param x        X coordinate within the window.
        /// @param y        Y coordinate within the window.
        /// @param single   true if the event is a single-click, else false.
        virtual void on_mouse_click_down(MouseButton, bool single, int x, int y) noexcept;

        /// Called when the mouse is (single or double) clicked and released.
        /// (0,0) is at the window top-left.
        /// @param x        X coordinate within the window.
        /// @param y        Y coordinate within the window.
        /// @param single   true if the event is a single-click, else false.
        virtual void on_mouse_click_up(MouseButton, bool single, int x, int y) noexcept;

        /// Called when the window size is changed, called with the new width
        /// and height.
        /// @param width    The new window width, in pixels.
        /// @param height   The new window height, in pixels.
        virtual void on_window_size_change(unsigned int width, unsigned int height) noexcept;

        /// Called when the game is about to shutdown, as a result of the
        /// `quit_game` call being made.
        virtual void on_quit() noexcept;

        /// Called prior to the first ever `update` call, this is intended to
        /// allow the screen to do any one-time initialisation. This call
        /// will be called once per usage of a screen in a Game update loop, and
        /// thus may be called multiple times if this screen is re-used (for
        /// example if it is selected for use again by a `get_next_screen` call.
        /// `prepare` may be used to set OpenGL state variables: the system
        /// guarantees that no other calls to OpenGL shall be made between this
        /// call to `prepare` and the other calls to this object. No other
        /// object in the system shall be modifying or using OpenGL other than
        /// this screen object.
        virtual void prepare(std::shared_ptr<GameGL>);

        /// Call to update the game state. Returns the action the loop should
        /// take e.g. exit the loop, continue the loop, or use a new screen.
        /// @param elapsed_ms   How many milliseconds have elapsed since the
        ///                     previous call to `update`. The first call is
        ///                     guaranteed to have this value as zero.
        /// @return             Returns the Action the loop should take.
        virtual Action update( std::uint64_t elapsed_ms );

        /// Render the current state of the application. If this returns true
        /// then the system will call a swap the back buffer.
        /// @return             Return true to request a window back-buffer swap.
        virtual bool render();

        /// Returns the next screen the loop should use (or an empty shared_ptr).
        /// @return A shared pointer to the next screen to use for the loop,
        ///         or possibly an empty shared pointer (signalling to exit the
        ///         loop) if the `update` call had not returned `ChangeScreen`.
        virtual std::shared_ptr<Screen> get_next_screen() const noexcept;

    protected:

        Screen() = default;
    };
}

#endif
