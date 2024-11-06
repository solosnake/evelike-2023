#include <atomic>
#include <stdexcept>
#include <thread>
#include <cstdlib>
#include "SDL.h"
#include "solosnake/gamegl.hpp"
#include "solosnake/screen.hpp"

namespace solosnake
{
    // We rely on the enum and the SDL mouse button values relationships:
    static_assert(
        SDL_BUTTON_LEFT == 1 + static_cast<int>(Screen::MouseButton::Left),
        "Wrong left button value");
    static_assert(
        SDL_BUTTON_MIDDLE == 1 + static_cast<int>(Screen::MouseButton::Middle),
        "Wrong middle button value");
    static_assert(
        SDL_BUTTON_RIGHT == 1 + static_cast<int>(Screen::MouseButton::Right),
        "Wrong right button value");

    class GameGL::Internal
    {
    public:

        Internal() = default;

        ~Internal() noexcept;

        void init_sdl(const char* organisation_name,
                      const char* application_name);

        void make_main_window(const std::optional<Image>& icon,
                              const std::string_view& title);

        void show_main_window();

        void toggle_main_window_fullscreen();

        void destroy_main_window() noexcept;

        std::shared_ptr<Screen> run_screen_loop(std::shared_ptr<GameGL>,
                                                std::shared_ptr<Screen>);

        void enable_gl_debugging();

        void disable_gl_debugging();

        std::shared_ptr<OpenGL>         m_opengl;
        std::string                     m_base_path;
        std::string                     m_pref_path;
        SDL_Window*                     m_main_window{nullptr};
        SDL_GLContext                   m_main_context{nullptr};
        bool                            m_already_run{false};
        bool                            m_exit_game{false};
    };

    namespace
    {
        /// Exception that captures the current SDL error in the message (if any).
        class SDL_exception final : public std::runtime_error
        {
        public:
            explicit SDL_exception(const char* msg);
        };

        // ---------------------------------------------------------------------
        std::pair<int,int>      get_screen_width_and_height();
        void                    destroy_surface(SDL_Surface*) noexcept;
        void                    set_sdl_gl_version(const std::pair<int,int>&);
        bool                    is_good_image(const Image&) noexcept;
        unsigned int            get_height(const Image&);
        void                    set_icon(const Image&, SDL_Window*);
        std::unique_ptr<SDL_Surface, decltype(&destroy_surface)> make_image_surface(const Image&);

        // ---------------------------------------------------------------------

        struct RGB { Uint8 r; Uint8 g; Uint8 b; };

        using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&destroy_surface)>;

        SDL_exception::SDL_exception(const char* msg)
            : std::runtime_error(std::string(msg)
                            + ": '"
                            + std::string(SDL_GetError())
                            + "'")
        {
        }

        void destroy_surface(SDL_Surface* p) noexcept
        {
            if (p)
            {
                SDL_FreeSurface(p);
            }
        }

        SurfacePtr make_image_surface(const Image& image)
        {
            constexpr Uint32 red_mask   = 0xff000000;
            constexpr Uint32 green_mask = 0x00ff0000;
            constexpr Uint32 blue_mask  = 0x0000ff00;
            constexpr Uint32 alpha_mask = 0x000000ff;
            constexpr int depth  = 32;

            const int h      = static_cast<int>(get_height(image));
            const int w      = static_cast<int>(image.width());
            const int pitch  = static_cast<int>(image.width() *  4);

            auto surface = SDL_CreateRGBSurfaceFrom(const_cast<std::uint8_t* >(image.pixels()),
                                                    w, h,
                                                    depth,
                                                    pitch,
                                                    red_mask,
                                                    green_mask,
                                                    blue_mask,
                                                    alpha_mask);

            if (nullptr == surface)
            {
                throw SDL_exception("CreateRGBSurface failed");
            }

            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

            return SurfacePtr(surface, destroy_surface);
        }

        void set_sdl_gl_version(const std::pair<int,int>& version)
        {
            if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version.first))
            {
                throw SDL_exception("GL_SetAttribute MAJOR VERSION failed");
            }

            if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version.second))
            {
                throw SDL_exception("GL_SetAttribute MINOR VERSION failed");
            }

            if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
            {
                throw SDL_exception("GL_SetAttribute PROFILE MASK failed");
            }
        }

        /// Returns true only if @a img can be a valid rectangular image.
        bool is_good_image(const Image& img) noexcept
        {
            return img.width() > 0u and img.pixels_count() > 0u and (0 == img.pixels_count() % img.width());
        }

        /// Returns the height of @a img if it is considered a valid Image
        /// (width * height = number of pixels).
        unsigned int get_height(const Image& img)
        {
            if(is_good_image(img))
            {
                return static_cast<unsigned int>(img.pixels_count() / img.width());
            }
            else
            {
                throw std::runtime_error("Image is not a rectangle w.r.t. its width.");
            }
        }

        void set_icon(const Image& icon, SDL_Window* window)
        {
            if(is_good_image(icon))
            {
                auto surface = make_image_surface(icon);
                SDL_SetWindowIcon(window, surface.get());
            }
        }

        std::pair<int,int> get_screen_width_and_height()
        {
            SDL_DisplayMode dm;

            if (SDL_GetDesktopDisplayMode(0,& dm) != 0)
            {
                throw SDL_exception("GetDesktopDisplayMode failed");
            }

            if( 0 == dm.w or 0 == dm.h )
            {
                throw std::runtime_error("Desktop has zero width or height.");
            }

            return std::make_pair(dm.w, dm.h);
        }
    }

    //--------------------------------------------------------------------------


    GameGL::Internal::~Internal() noexcept
    {
        m_already_run = true;
        m_exit_game = true;
        destroy_main_window();
    }

    void GameGL::Internal::enable_gl_debugging()
    {
        m_opengl->EnableDebugOutput();
    }

    void GameGL::Internal::disable_gl_debugging()
    {
        m_opengl->DisableDebugOutput();
    }

    void GameGL::Internal::init_sdl(const char* organisation_name,
                                    const char* application_name)
    {
        if (m_exit_game || m_already_run)
        {
            throw std::runtime_error("Game has already been run.");
        }
        else
        {
            m_already_run = true;
        }

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw SDL_exception("SDL_Init video failed");
        }

        auto base = SDL_GetBasePath();
        m_base_path = base;
        SDL_free(base);
        SDL_Log( "Base path: %s", m_base_path.c_str());

        auto pref = SDL_GetPrefPath( organisation_name, application_name );
        m_pref_path = pref;
        SDL_free(pref);
        SDL_Log( "Pref path: %s", m_pref_path.c_str());
    }

    void GameGL::Internal::toggle_main_window_fullscreen()
    {
        if(m_main_window)
        {
            constexpr auto full = SDL_WINDOW_FULLSCREEN_DESKTOP;
            auto toggle = (SDL_GetWindowFlags(m_main_window) & full) ? 0 : full;
            SDL_SetWindowFullscreen(m_main_window, toggle);
        }
    }

    void GameGL::Internal::show_main_window()
    {
        SDL_ShowWindow(m_main_window);
        SDL_RaiseWindow(m_main_window);
        if(SDL_GL_MakeCurrent(m_main_window, m_main_context) < 0)
        {
            throw SDL_exception("GL_MakeCurrent failed");
        }
    }

    void GameGL::Internal::destroy_main_window() noexcept
    {
        if(m_main_context)
        {
            SDL_GL_DeleteContext(m_main_context);
            m_main_context = nullptr;
        }

        if (m_main_window)
        {
            SDL_DestroyWindow(m_main_window);
            m_main_window = nullptr;
        }
    }

    void GameGL::Internal::make_main_window(const std::optional<Image>& icon,
                                          const std::string_view& title)
    {
        auto screen_w_h = get_screen_width_and_height();
        SDL_Log("W %d H %d", screen_w_h.first, screen_w_h.second);

        // Create window (initially hidden until later loading phase is over).
        // Create a double-buffered OpenGL context by default.
        m_main_window = SDL_CreateWindow(title.data(),
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         screen_w_h.first, screen_w_h.second,
                                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

        if (nullptr == m_main_window)
        {
            throw SDL_exception("CreateWindow failed");
        }

        // Create OpenGL context (double-buffered by default).
        m_main_context = SDL_GL_CreateContext(m_main_window);
        if (nullptr == m_main_context)
        {
            throw SDL_exception("GL_CreateContext failed");
        }

        // Initialise OpenGL state.
        m_opengl = std::make_shared<OpenGL>();

        // Use adaptive Vsync (-1), or Vsync (1)
        if (SDL_GL_SetSwapInterval(-1) < 0 || SDL_GL_SetSwapInterval(1) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Unable to set Adaptive VSync or VSync! %s\n",
                         SDL_GetError());
        }

        // Optionally set an icon on the main window.
        if (icon)
        {
            set_icon(icon.value(), m_main_window);
        }
    }

    std::shared_ptr<Screen> GameGL::Internal::run_screen_loop(std::shared_ptr<GameGL> game,
                                                              std::shared_ptr<Screen> screen)
    {
        std::shared_ptr<Screen> next_screen;

        if( not screen )
        {
            // Handle empty screen - just return empty next screen.
            return next_screen;
        }
        else
        {
            screen->prepare(game);
        }

        // Do first call to `update` with 0 ms elapsed time:
        if( not m_exit_game )
        {
            if( Screen::Action::Quit == screen->update(0) )
            {
                m_exit_game = true;
            }
        }

        // See if this game has a maximum permitted uptime.
        const auto time_limit = game->get_max_runtime_ms();

        // How many milliseconds since SDL library initialized.
        const Uint64 start_time = SDL_GetTicks64();
        Uint64 timestamp = start_time;

        // Declare the event once: its quite a large structure.
        SDL_Event e;

        // While application is running
        while (not (m_exit_game or next_screen))
        {
            const auto now = SDL_GetTicks64();

            // Check how long has passed since last call to `update`.
            const auto elapsed_ms = now - timestamp;

            // Check how long has passed since we started running:
            const auto runtime = now - start_time;

            // Check to see if we have reached or exceeded out time limit.
            if(runtime >= time_limit)
            {
                m_exit_game = true;
            }

            // Only update if some time has passed.
            if( elapsed_ms > 0 )
            {
                timestamp = now;

                // Update screen and see what it wants done next.
                auto a = screen->update( elapsed_ms );

                switch( a )
                {
                    case Screen::Action::Continue:
                        if( screen->render() )
                        {
                            SDL_GL_SwapWindow(m_main_window);
                        }
                    break;

                    case Screen::Action::ChangeScreen:
                        next_screen = screen->get_next_screen();
                    break;

                    case Screen::Action::Quit:
                    default:
                        m_exit_game = true;
                    break;
                }
            }

            // Handle events on queue
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type)
                {
                case SDL_QUIT:
                    SDL_Log("SDL_QUIT, quitting");
                    m_exit_game = true;
                    break;

                case SDL_KEYDOWN:
                    screen->on_keydown(e.key.keysym.sym, e.key.keysym.mod);
                    break;

                case SDL_KEYUP:
                    screen->on_keyup(e.key.keysym.sym, e.key.keysym.mod);
                    break;

                case SDL_MOUSEMOTION:
                    screen->on_mouse_move(e.motion.x, e.motion.y);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button <= SDL_BUTTON_RIGHT)
                    {
                        auto button = static_cast<Screen::MouseButton>(e.button.button - 1);
                        screen->on_mouse_click_down(button, e.button.clicks == 1, e.button.x, e.button.y);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if(e.button.button <= SDL_BUTTON_RIGHT)
                    {
                        auto button = static_cast<Screen::MouseButton>(e.button.button - 1);
                        screen->on_mouse_click_up(button, e.button.clicks == 1, e.button.x, e.button.y);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    {
                        auto horizontal = e.wheel.preciseX;
                        auto vertical   = e.wheel.preciseY;
                        if(e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                        {
                            horizontal *= -1.0f;
                            vertical   *= -1.0f;
                        }
                        screen->on_mouse_wheel(horizontal, vertical);
                    }
                    break;

                case SDL_WINDOWEVENT:
                    switch(e.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                        SDL_Log("SDL_WINDOWEVENT_CLOSE");
                        m_exit_game = true;
                        break;

                        // The window has been moved to another display (data1).
                        case SDL_WINDOWEVENT_DISPLAY_CHANGED:
                        SDL_Log("SDL_WINDOWEVENT_DISPLAY_CHANGED");
                        break;

                        // Window has been shown.
                        case SDL_WINDOWEVENT_SHOWN:
                        SDL_Log("SDL_WINDOWEVENT_SHOWN");
                        break;

                        // Window has been hidden (opposite of shown).
                        case SDL_WINDOWEVENT_HIDDEN:
                        SDL_Log("SDL_WINDOWEVENT_HIDDEN");
                        break;

                        // The window size has changed, either as a result
                        // of an API call or through the system or user
                        // changing the window size.
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SDL_Log("SDL_WINDOWEVENT_SIZE_CHANGED");
                        screen->on_window_size_change(static_cast<unsigned int>(e.window.data1),
                                                      static_cast<unsigned int>(e.window.data2));
                        break;

                        // Window has been minimized
                        case SDL_WINDOWEVENT_MINIMIZED:
                        SDL_Log("SDL_WINDOWEVENT_MINIMIZED");
                        break;

                        // Window has been maximized
                        case SDL_WINDOWEVENT_MAXIMIZED:
                        SDL_Log("SDL_WINDOWEVENT_MAXIMIZED");
                        break;

                        // Window has had normal size & position restored.
                        case SDL_WINDOWEVENT_RESTORED:
                        SDL_Log("SDL_WINDOWEVENT_RESTORED");
                        break;

                        default:
                        break;
                    }
                }
            }
        }

        if( m_exit_game )
        {
            next_screen = nullptr;
        }

        return next_screen;
    }

    //--------------------------------------------------------------------------

    void GameGL::log_info(const std::string_view& text) noexcept
    {
        SDL_Log("%s", text.data());
    }

    void GameGL::log_error(const std::string_view& text) noexcept
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", text.data());
    }

    void GameGL::set_window_title_text(const std::string_view& text) noexcept
    {
        if (m_internal->m_main_window)
        {
            SDL_SetWindowTitle(m_internal->m_main_window, text.data());
        }
    }

    int GameGL::get_window_width() const noexcept
    {
        int w{-1};
        if (m_internal->m_main_window)
        {
            SDL_GL_GetDrawableSize(m_internal->m_main_window, &w, nullptr);
        }
        return w;
    }

    int GameGL::get_window_height() const noexcept
    {
        int h{-1};
        if (m_internal->m_main_window)
        {
            SDL_GL_GetDrawableSize(m_internal->m_main_window, nullptr, &h);
        }
        return h;
    }

    std::optional<Image> GameGL::get_window_icon_image() noexcept
    {
        std::optional<Image> empty;
        return empty;
    }


    std::uint64_t GameGL::get_max_runtime_ms() const noexcept
    {
        return std::numeric_limits<std::uint64_t>::max();
    }

    GameGL::GameGL() : m_internal(std::make_unique<GameGL::Internal>())
    {
    }

    GameGL::~GameGL() noexcept
    {
        m_internal.reset(nullptr);
    }

    std::string GameGL::get_application_data_directory() const
    {
        SDL_assert( m_internal->m_already_run );
        return m_internal->m_base_path;
    }

    std::string GameGL::get_application_pref_directory() const
    {
        SDL_assert( m_internal->m_already_run );
        return m_internal->m_pref_path;
    }

    void GameGL::toggle_fullscreen()
    {
        m_internal->toggle_main_window_fullscreen();
    }

    const OpenGL* GameGL::opengl() const noexcept
    {
        return m_internal->m_opengl.get();
    }

    OpenGL* GameGL::opengl() noexcept
    {
        return m_internal->m_opengl.get();
    }

    std::shared_ptr<OpenGL> GameGL::opengl_ptr() noexcept
    {
        return m_internal->m_opengl;
    }

    std::shared_ptr<GameGL> GameGL::get_shared_ptr()
    {
        return shared_from_this();
    }

    int GameGL::run_game_ptr(std::shared_ptr<GameGL> g) noexcept
    {
        if(not g)
        {
            return EXIT_FAILURE;
        }

        int result{EXIT_FAILURE};

        try
        {
            g->m_internal->init_sdl( g->get_organisation_name().c_str(),
                                     g->get_application_name().c_str() );

            auto title    = g->get_window_title();
            auto icon     = g->get_window_icon_image();

            // Make OpenGL window initially not visible).
            g->m_internal->make_main_window(icon, title);

            // Debugging is enabled by default during startup.
            g->m_internal->enable_gl_debugging();

            // Request GL core version.
            set_sdl_gl_version(g->get_gl_version());

            // Make OpenGL window initially not visible).
            g->m_internal->make_main_window(icon, title);

            // Show main window, closes optional splash windows.
            g->m_internal->show_main_window();

            // Debugging is disabled before first screen. The user can enable
            // or disable it then.
            g->m_internal->disable_gl_debugging();

            // Get the first screen
            auto screen = g->get_initial_screen();

            while(screen)
            {
                screen = g->m_internal->run_screen_loop( g, screen );
            }
        }
        catch (const SDL_exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL Exception: %s", e.what());
        }
        catch (const std::exception& e)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exception: %s", e.what());
        }
        catch (...)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown exception.");
        }

        SDL_Quit();

        return result;
    }
}
