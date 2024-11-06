#include <array>
#include <cassert>
#include <filesystem>
#include "SDL.h"
#include "solosnake/opengl.hpp"
#include "solosnake/gamegl.hpp"
#include "solosnake/evelike-screen-loading.hpp"
#include "solosnake/evelike-screen-universe.hpp"
#include "solosnake/evelike-screen-failure.hpp"

namespace solosnake::evelike
{
    namespace
    {
        class Task
        {
        public:
            Task(std::shared_ptr<EveLike_game>);
            virtual ~Task() = default;
            virtual Loading_screen::TaskStatus update() noexcept = 0;
        protected:
            std::shared_ptr<EveLike_game>   m_game;
        };

        class FirstScreen final : public Task
        {
        public:
            FirstScreen(std::shared_ptr<EveLike_game>);
            Loading_screen::TaskStatus update() noexcept final;
            uint8_t m_countdown{3u};
        };

        class LoadUniverseJson final : public Task
        {
        public:
            LoadUniverseJson(std::shared_ptr<EveLike_game>);
            Loading_screen::TaskStatus update() noexcept final;
        };

        class LoadUniverseGraphics final : public Task
        {
        public:
            LoadUniverseGraphics(std::shared_ptr<EveLike_game>);
            Loading_screen::TaskStatus update() noexcept final;
        };

        class LoadSolarSystemGraphics final : public Task
        {
        public:
            LoadSolarSystemGraphics(std::shared_ptr<EveLike_game>);
            Loading_screen::TaskStatus update() noexcept final;
        };

        class LoadLocationGraphics final : public Task
        {
        public:
            LoadLocationGraphics(std::shared_ptr<EveLike_game>);
            Loading_screen::TaskStatus update() noexcept final;
        };

        //----------------------------------------------------------------------

        Task::Task(std::shared_ptr<EveLike_game> g)
        : m_game(g)
        {
            assert(g);
        }


        FirstScreen::FirstScreen(std::shared_ptr<EveLike_game> g)
        : Task(g)
        {
        }

        LoadUniverseJson::LoadUniverseJson(std::shared_ptr<EveLike_game> g)
        : Task(g)
        {
        }

        LoadUniverseGraphics::LoadUniverseGraphics(std::shared_ptr<EveLike_game> g)
        : Task(g)
        {
        }

        LoadSolarSystemGraphics::LoadSolarSystemGraphics(std::shared_ptr<EveLike_game> g)
        : Task(g)
        {
        }

        LoadLocationGraphics::LoadLocationGraphics(std::shared_ptr<EveLike_game> g)
        : Task(g)
        {
        }

        Loading_screen::TaskStatus FirstScreen::update() noexcept
        {
            if( m_countdown > 0u )
            {
                --m_countdown;
                return Loading_screen::TaskStatus::Running;
            }
            else
            {
                return Loading_screen::TaskStatus::Succeeded;
            }
        }

        Loading_screen::TaskStatus LoadUniverseJson::update() noexcept
        {
            auto result = Loading_screen::TaskStatus::Failed;

            auto d = m_game->game_data();
            assert(d);

            try
            {
                d->universe.reset();
                d->universe = std::make_shared<Universe>(d->universe_file.c_str());
                m_game->log_info("Loaded universe: '" + d->universe_file + "'.");
                result = Loading_screen::TaskStatus::Succeeded;
            }
            catch(const std::exception& e)
            {
                m_game->log_error("Failed to load universe '" +
                                  d->universe_file + "'" +
                                  e.what() + "'.");
            }
            catch(...)
            {
                m_game->log_error("Failed to load universe '" +
                                  d->universe_file + "'.");
            }

            return result;
        }

        Loading_screen::TaskStatus LoadUniverseGraphics::update() noexcept
        {
            auto result = Loading_screen::TaskStatus::Succeeded;
            return result;
        }

        Loading_screen::TaskStatus LoadSolarSystemGraphics::update() noexcept
        {
            auto result = Loading_screen::TaskStatus::Succeeded;
            return result;
        }

        Loading_screen::TaskStatus LoadLocationGraphics::update() noexcept
        {
            auto result = Loading_screen::TaskStatus::Succeeded;
            return result;
        }
    }

    Loading_screen::Loading_screen(std::shared_ptr<EveLike_game> g)
        : Evelike_screen(g)
        , m_opengl(g->opengl())
        , m_next_screen()
        , m_tasks()
        , m_loading_status(Loading_screen::TaskStatus::NotStarted)
        , m_quit(false)
    {
        assert( g );
        assert( m_opengl );
    }

    Loading_screen::~Loading_screen()
    {
        drain_task_stack();
    }

    void Loading_screen::on_window_size_change(const unsigned int width,
                                               const unsigned int height) noexcept
    {
        resize_viewport(width, height);
    }

    void Loading_screen::prepare()
    {
        resize_viewport();
        m_opengl->EnableDepthTest(true);
        m_opengl->EnableBackFaceCulling(false);
        m_opengl->EnableLinePolygonMode(true);
        m_opengl->ClearColor(0.0f, 0.9f, 0.0f, 1.0f);
    }

    void Loading_screen::on_keydown(const std::int32_t key,
                                    const std::uint16_t ) noexcept
    {
        switch(key)
        {
        case SDLK_q:
            m_quit = true;
            break;

        default:
            break;
        }
    }

    void Loading_screen::on_quit() noexcept
    {
        m_quit = true;
    }

    void Loading_screen::build_task_stack()
    {
        assert(m_tasks.empty());

        auto g         = get_game_ptr();
        auto first     = std::make_shared<FirstScreen>(g);
        auto load_json = std::make_shared<LoadUniverseJson>(g);
        auto uni_gfx   = std::make_shared<LoadUniverseGraphics>(g);
        auto ss_gfx    = std::make_shared<LoadSolarSystemGraphics>(g);
        auto loc_gfx   = std::make_shared<LoadLocationGraphics>(g);

        // Queue is first-in-first-out.
        m_tasks.push( [first]    (){ return first->update();     } );
        m_tasks.push( [load_json](){ return load_json->update(); } );
        m_tasks.push( [uni_gfx]  (){ return uni_gfx->update();   } );
        m_tasks.push( [ss_gfx]   (){ return ss_gfx->update();    } );
        m_tasks.push( [loc_gfx]  (){ return loc_gfx->update();   } );
    }

    void Loading_screen::drain_task_stack() noexcept
    {
        try
        {
            while(not m_tasks.empty())
            {
                m_tasks.pop();
            }
        }
        catch(...)
        {
            log_error("Failed to empty tasks.");
        }
    }

    bool Loading_screen::render()
    {
        m_opengl->Clear(OpenGL::COLOR_BUFFER_BIT | OpenGL::DEPTH_BUFFER_BIT);
        return true;
    }

    Screen::Action Loading_screen::update( std::uint64_t )
    {
        Screen::Action result = Screen::Action ::Continue;

        switch( m_loading_status )
        {
            case Loading_screen::TaskStatus::NotStarted:
            {
                // Build list of tasks.
                assert( m_tasks.empty() );
                build_task_stack();
                if(m_tasks.empty())
                {
                    m_loading_status = Loading_screen::TaskStatus::Failed;
                }
                else
                {
                    m_loading_status = Loading_screen::TaskStatus::Running;
                }
            }
            break;

            case Loading_screen::TaskStatus::Running:
            {
                assert(not m_tasks.empty());
                // Call first functor.
                auto task_status = m_tasks.front()();
                switch(task_status)
                {
                    case Loading_screen::TaskStatus::Failed:
                        m_loading_status = Loading_screen::TaskStatus::Failed;
                        break;

                    case Loading_screen::TaskStatus::Succeeded:
                        m_tasks.pop();
                        if(m_tasks.empty())
                        {
                            // If there are no more tasks then we are finished.
                            m_loading_status = Loading_screen::TaskStatus::Succeeded;
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

            case Loading_screen::TaskStatus::Succeeded:
            {
                // All loading tasks completed. Queue up the next screen.
                m_next_screen = std::make_unique<Universe_screen>(get_game_ptr());
                result = Screen::Action::ChangeScreen;
            }
            break;

            case Loading_screen::TaskStatus::Failed:
            {
                drain_task_stack();
                // A loading task failed. Queue up the failure screen.
                m_next_screen = std::make_unique<Failure_screen>(get_game_ptr());
                result = Screen::Action::ChangeScreen;
            }
            break;
        }

        if( m_quit )
        {
            result = Screen::Action::Quit;
            drain_task_stack();
        }

        return result;
    }

    std::shared_ptr<Screen> Loading_screen::get_next_screen() const noexcept
    {
        return std::shared_ptr<Screen>(m_next_screen.release());
    }
}
