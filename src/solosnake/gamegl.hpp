#ifndef SOLOSNAKE_GAMEGL_HPP
#define SOLOSNAKE_GAMEGL_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "solosnake/image.hpp"
#include "solosnake/log.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/screen.hpp"

namespace solosnake
{
    /// Base class for an Game-like application where the main loop will want to
    /// run as fast as possible, updating and rendering often, as opposed to an
    /// event driven application like a tool. This class must provide
    /// information to the underlying system, and provide a first Screen to
    /// show.
    /// This class implements the `Log` interface to permit the Log to be passed
    /// to other instances or methods.
    class GameGL : public std::enable_shared_from_this<GameGL>,
                   public Log
    {
    public:

        /// Inherit from `Game` and instance a `Game` and pass it in here to run
        /// a game. It will load the assets and run until the update loop
        /// returns `false` or `Quit` is detected. This will trigger the SDL
        /// assets to be created, based on the how the inheriting class has been
        /// defined.
        ///
        /// @param args The arguments used to construct a GAME shared_ptr.
        /// @return     The exit code for the application, either EXIT_SUCCESS
        ///             or EXIT_FAILURE.
        template <typename GAME, typename... Args>
        static int run_game(Args... args)
        {
            auto g = std::make_shared<GAME>(args...);
            return GameGL::run_game_ptr(g);
        }

        /// Cleans up the SDL assets.
        virtual ~GameGL() noexcept;

        /// Writes the given informational text to the log system. Typically
        /// uses the SDL2 log for this.
        ///
        /// @param text The informational text to write to the log.
        void log_info(const std::string_view& text) noexcept final;

        /// Writes the given error text to the log system. Typically uses the
        /// SDL2 log for this.
        ///
        /// @param text The error text to write to the log.
        void log_error(const std::string_view& text) noexcept final;

        /// Sets the window title text (which might not be visible).
        /// @param text The text to use for the window title.
        void set_window_title_text(const std::string_view& text) noexcept;

        /// Returns the width of the window, in pixels. Returns -1 if there is
        /// no window, or an error occurred.
        int get_window_width() const noexcept;

        /// Returns the height of the window, in pixels. Returns -1 if there is
        /// no window, or an error occurred.
        int get_window_height() const noexcept;

        /// Returns the operating system specific absolute path for the
        /// applications read-only files directory.
        /// @pre May only be called from within `run_game`.
        std::string get_application_data_directory() const;

        /// Returns the operating system specific absolute path for the
        /// applications runtime writable files e.g. logs, data, saves etc.
        /// e.g. "C:\\Users\\bob\\AppData\\Roaming\\My Company\\My Program Name\\"
        ///   or "/home/bob/.local/share/My Program Name/"
        /// @pre May only be called from within `run_game`.
        std::string get_application_pref_directory() const;

        /// Returns a const pointer to the managed OpenGL wrapper.
        const OpenGL* opengl() const noexcept;

        /// Returns a pointer to the managed OpenGL wrapper.
        OpenGL* opengl() noexcept;

        /// Returns the managed OpenGL wrapper.
        std::shared_ptr<OpenGL> opengl_ptr() noexcept;

        /// Toggles the main application window between fullscreen and windowed.
        void  toggle_fullscreen();

    protected:

        /// Constructs the Game, but does not yet interact with SDL. No assets
        /// are created at this point.
        GameGL();

        /// Returns the this pointer as a managed pointer.
        std::shared_ptr<GameGL> get_shared_ptr();

        template<typename T>
        std::shared_ptr<T> get_dynamic_shared_ptr()
        {
            return std::dynamic_pointer_cast<T>( get_shared_ptr() );
        }

    private:

        /// @Runs the game object.
        static int run_game_ptr(std::shared_ptr<GameGL>) noexcept;

        /// Requests the initial screen to use. This will only be called once
        /// by the system, after the SDL has been initialised and after the
        /// main window has been created. The current Game is provided to allow
        /// the user to inspect settings, or log etc.
        /// @param game A shared pointer to the application game.
        virtual std::shared_ptr<Screen> get_initial_screen() = 0;

        /// The user should return the preferred major/minor OpenGL core version
        /// the system should try and create.
        /// @return A pair containing the user's preferred major/minor OpenGL
        ///         core version.
        virtual std::pair<int,int>      get_gl_version() noexcept = 0;

        /// The user should return the applications name. This should be a
        /// simple ASCII string. This string will be used internally by the OS.
        /// @return Text to be used as the application name within the OS.
        virtual std::string             get_application_name() noexcept = 0;

        /// The user should return the organisations name. This should be a
        /// simple ASCII string. This string will be used internally by the OS.
        /// @return Text to be used as the organisations name within the OS.
        virtual std::string             get_organisation_name() noexcept = 0;

        /// The user should return the (initial) window title text.
        /// @return Text to be used as the window title text.
        virtual std::string             get_window_title() noexcept = 0;

        /// If the user returns an image to use as the window icon. Defaults to
        /// no Image.
        /// @return An optional Image to use as the window icon.
        virtual std::optional<Image>    get_window_icon_image() noexcept;

        /// Returns the maximum number of milliseconds the application shall
        /// be permitted to run for. Returns max uint64 by default. As no runtime
        /// can ever exceed this, max(uint64) effectively disables this limit.
        virtual std::uint64_t           get_max_runtime_ms() const noexcept;

    private:

        class Internal;
        std::unique_ptr<Internal> m_internal;
    };
}

#endif // SOLOSNAKE_GAME_HPP
