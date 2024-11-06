#include <cassert>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include "SDL.h"
#include "solosnake/opengl.hpp"
#include "solosnake/evelike-game.hpp"
#include "solosnake/evelike-icon.hpp"
#include "solosnake/evelike-show-model.hpp"
#include "solosnake/evelike-view-gl-state.hpp"
#include "solosnake/file.hpp"
#include "solosnake/meshgl.hpp"
#include "solosnake/screen.hpp"
#include "solosnake/shadergl.hpp"
#include "solosnake/skybox.hpp"

namespace solosnake::evelike
{
    namespace
    {
        // How many debugging views are supported by the OpenGL shader.
        constexpr auto SHADER_DEBUG_VIEWS = 18u;

        /// Returns a file friendly string with the current time and date.
        std::string time_stamp()
        {
            auto now = std::chrono::system_clock::now();
            auto ms  = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            std::stringstream ss;
            ss << ms;
            return ss.str();
        }

        struct Show_model_data
        {
            std::string     model_file;
            std::string     skybox_file;
            std::uint64_t   max_milliseconds {0u};
            bool            debugging        {false};
        };

        class Show_model_game final : public solosnake::GameGL
        {
        public:
            Show_model_game(const std::string_view& model_file,
                            const std::string_view& skybox_file,
                            bool debugging,
                            std::uint64_t max_milliseconds);
            ~Show_model_game();
            Show_model_data*        get_data();
        private:
            std::shared_ptr<Screen> get_initial_screen() final;
            std::string             get_application_name() noexcept final;
            std::string             get_organisation_name() noexcept final;
            std::pair<int,int>      get_gl_version() noexcept final;
            std::string             get_window_title() noexcept final;
            std::optional<Image>    get_window_icon_image() noexcept;
        private:
            std::shared_ptr<Show_model_data>  m_model_data;
        };

        class Show_model_screen final : public solosnake::Screen
        {
        public:
            explicit Show_model_screen(std::shared_ptr<Show_model_game>);
            ~Show_model_screen();
            void on_keydown(std::int32_t, std::uint16_t) noexcept final;
            void on_keyup(std::int32_t, std::uint16_t) noexcept final;
            void on_mouse_move(int x, int y) noexcept final;
            void on_mouse_click_down(MouseButton, bool single, int x, int y) noexcept final;
            void on_mouse_click_up(MouseButton, bool single, int x, int y) noexcept final;
            void on_window_size_change(unsigned int, unsigned int) noexcept final;
            void on_quit() noexcept final;
            void prepare(std::shared_ptr<GameGL>) final;
            Action update(std::uint64_t) final;
            bool render() final;
            std::shared_ptr<Screen> get_next_screen() const noexcept final;
            void resize_viewport(unsigned int width, unsigned int height) noexcept;
            Show_model_data* get_data();
        private:
            void take_screenshot() noexcept;
        private:
            std::filesystem::path                   m_assets_dir;
            std::filesystem::path                   m_screenshot_dir;
            std::unique_ptr<solosnake::MeshGL>      m_mesh;
            std::unique_ptr<solosnake::Skybox>      m_skybox;
            std::unique_ptr<solosnake::ShaderGL>    m_mesh_shader;
            std::shared_ptr<Show_model_game>        m_game;
            OpenGL*                                 m_opengl;
            View_gl_state                           m_view_gl_state;
            Matrix4x4                               m_scene_mat4;
            std::uint64_t                           m_milliseconds_uptime;
            int                                     m_uniform_model_view_matrix;
            int                                     m_uniform_projection_matrix;
            int                                     m_uniform_model_view_projection_matrix;
            int                                     m_uniform_light_view_position;
            int                                     m_uniform_debug_output;
            int                                     m_debug_output_value;
            float                                   m_camera_axis_length;
            float                                   m_light_axis_length;
            float                                   m_model_rotation_y;
            float                                   m_model_rotation_rate;
            float                                   m_light_rotation_y;
            float                                   m_light_rotation_rate;
            bool                                    m_rotate_model;
            bool                                    m_rotate_light;
            bool                                    m_quit;
        };

        //----------------------------------------------------------------------

        Show_model_screen::Show_model_screen(std::shared_ptr<Show_model_game> game)
        : m_mesh()
        , m_skybox()
        , m_mesh_shader()
        , m_game(game)
        , m_opengl(game->opengl())
        , m_view_gl_state()
        , m_scene_mat4(Identity4x4())
        , m_milliseconds_uptime(0u)
        , m_uniform_model_view_matrix{0}
        , m_uniform_projection_matrix{0}
        , m_uniform_model_view_projection_matrix{0}
        , m_uniform_light_view_position{0}
        , m_uniform_debug_output{0}
        , m_debug_output_value{0}
        , m_camera_axis_length{2.5f}
        , m_light_axis_length{5.0f}
        , m_model_rotation_y{0.0f}
        , m_model_rotation_rate{0.3f}
        , m_light_rotation_y{0.0f}
        , m_light_rotation_rate{0.5f}
        , m_rotate_model{false}
        , m_rotate_light{true}
        , m_quit(false)
        {
            if(not game->get_data()->model_file.empty())
            {
                m_mesh = std::make_unique<MeshGL>(game->opengl_ptr(),
                                                  game->get_data()->model_file);
            }

            if(not game->get_data()->skybox_file.empty())
            {
                m_skybox = std::make_unique<Skybox>(game->get_data()->skybox_file.c_str(),
                                                    game->opengl_ptr());
            }
        }

        Show_model_screen::~Show_model_screen()
        {
        }

        void Show_model_screen::take_screenshot() noexcept
        {
            try
            {
                if(not std::filesystem::exists(m_screenshot_dir))
                {
                    std::filesystem::create_directories(m_screenshot_dir);
                }

                auto file_name = (m_screenshot_dir / ("Screenshot_" + time_stamp() + ".tga")).string();
                m_opengl->SaveTGAScreenshot( file_name.c_str() );
                m_game->log_info( "Screenshot saved as '" + file_name + "'." );
            }
            catch(const std::exception& e)
            {
                m_game->log_error( std::string("Screenshot failed: ") + e.what());
            }
        }

        void Show_model_screen::on_keydown(const std::int32_t key,
                                           const std::uint16_t ) noexcept
        {
            switch(key)
            {
            case SDLK_w:
                m_opengl->ToggleLinePolygonMode();
                break;

            case SDLK_q:
                m_quit = true;
                break;

            case SDLK_r:
                m_rotate_model = not m_rotate_model;
                break;

            case SDLK_t:
                m_rotate_light = not m_rotate_light;
                break;

            case SDLK_u:
                m_debug_output_value = (1 + m_debug_output_value) % SHADER_DEBUG_VIEWS;
                break;

            case SDLK_i:
                m_debug_output_value = (SHADER_DEBUG_VIEWS -1 + m_debug_output_value) % SHADER_DEBUG_VIEWS;
                break;

            case SDLK_F11:
                m_game->toggle_fullscreen();
                break;

            case SDLK_F12:
                take_screenshot();
                break;

            default:
                break;
            }
        }

        void Show_model_screen::on_keyup(const std::int32_t,
                                         const std::uint16_t) noexcept
        {
        }

        void Show_model_screen::on_quit() noexcept
        {
            m_quit = true;
        }

        void Show_model_screen::on_window_size_change(const unsigned int width,
                                                      const unsigned int height) noexcept
        {
            resize_viewport(width, height);
        }

        void Show_model_screen::resize_viewport(const unsigned int width,
                                                const unsigned int height) noexcept
        {
            m_opengl->SetViewport(0, 0, width, height);
            m_view_gl_state.view_projection.set_viewport(width, height);
            m_view_gl_state.mouse_ball.set_window_size(width, height);
        }

        void Show_model_screen::prepare(std::shared_ptr<GameGL> g)
        {
            assert(g);
            assert(m_opengl);

            const auto ww = g->get_window_width();
            const auto wh = g->get_window_height();

            resize_viewport(ww, wh);

            m_opengl->EnableDepthTest(true);
            m_opengl->EnableBackFaceCulling(true);
            m_opengl->EnableLinePolygonMode(false);
            m_opengl->ClearColor(0.3f, 0.52f, 0.2f, 1.0f);
            m_opengl->EnableDebugOutput();

            float radius = 1.0f;
            auto camera_look_at = Point3d{ 0.0f, 0.0f, 0.0f };

            if(m_mesh)
            {
                radius = m_mesh->bounding_radius();
                camera_look_at = to_point(m_mesh->centre());
            }

            m_camera_axis_length = 1.1f * radius;
            g->log_info(std::string("Bounding radius is ") + std::to_string(radius));

            // Camera is looking at mesh centre one axis length away up Z axis.
            auto camera_position = camera_look_at + Point3d{ 0.0f, 0.0f, m_camera_axis_length };
            auto z_far           = m_camera_axis_length * 2.0f;

            m_view_gl_state.mouse_ball.set_window_size(ww, wh);
            m_view_gl_state.view_projection.set_viewport(ww, wh);
            m_view_gl_state.view_projection.set_camera_position(camera_position.x, camera_position.y, camera_position.z);
            m_view_gl_state.view_projection.set_camera_look_at(camera_look_at.x, camera_look_at.y, camera_look_at.z);
            m_view_gl_state.view_projection.set_z_near_far(0.1f, z_far);
            m_view_gl_state.view_projection.update();

            m_assets_dir = std::filesystem::path(g->get_application_data_directory()) / "assets";
            m_screenshot_dir = std::filesystem::path(g->get_application_pref_directory()) / "screenshots";
            g->log_info(std::string("Assets dir: ") + m_assets_dir.string());
            g->log_info(std::string("Screenshots dir: ") + m_screenshot_dir.string());

            // Make shader
            auto vert     = get_file_contents((m_assets_dir / "shaders" / "debug_mesh.vert").string());
            auto frag     = get_file_contents((m_assets_dir / "shaders" / "debug_mesh.frag").string());
            m_mesh_shader = std::make_unique<ShaderGL>(m_game->opengl_ptr(), vert, frag);

            // Get uniform locations.
            m_uniform_model_view_matrix             = m_mesh_shader->get_uniform_location("model_view_matrix");
            m_uniform_model_view_projection_matrix  = m_mesh_shader->get_uniform_location("model_view_projection_matrix");
            m_uniform_light_view_position           = m_mesh_shader->get_uniform_location("light_view_position");
            m_uniform_debug_output                  = m_mesh_shader->get_uniform_location("debug_output");
        }

        Screen::Action Show_model_screen::update(const std::uint64_t ms)
        {
            constexpr float Pi2 { 2.0f * 3.1415926535898f };

            if( m_rotate_model )
            {
                m_model_rotation_y += m_model_rotation_rate * ms * 0.001f;
            }

            if( m_rotate_light )
            {
                m_light_rotation_y += m_light_rotation_rate * ms * 0.001f;
            }

            while(m_model_rotation_y > Pi2)
            {
                m_model_rotation_y -= Pi2;
            }

            while(m_light_rotation_y > Pi2)
            {
                m_light_rotation_y -= Pi2;
            }

            m_view_gl_state.view_projection.update();

            // Check if there is a time limit on how long this screen
            // should be displayed for.
            auto action = Screen::Action::Continue;

            if(get_data()->max_milliseconds > 0u)
            {
                m_milliseconds_uptime += ms;

                if(m_milliseconds_uptime > get_data()->max_milliseconds)
                {
                    action = Screen::Action::Quit;
                }
            }

            return m_quit ? Screen::Action::Quit : action;
        }

        bool Show_model_screen::render()
        {
            // Combine current scene transform with the mouseball rotation.
            auto drag  = m_view_gl_state.mouse_ball.get_drag_rotation();
            Matrix4x4 scene_and_drag;
            mul_4x4( drag.data(), m_scene_mat4, static_cast<float*>(scene_and_drag) );
            auto model = rotation_4x4_y( m_model_rotation_y );
            Matrix4x4 scene_and_drag_and_model;
            mul_4x4( scene_and_drag, model, static_cast<float*>(scene_and_drag_and_model) );
            auto view       = m_view_gl_state.view_projection.get_model_view(scene_and_drag);
            auto model_view = m_view_gl_state.view_projection.get_model_view(scene_and_drag_and_model);
            auto mvp        = m_view_gl_state.view_projection.get_model_view_projection(scene_and_drag_and_model);

            if(m_skybox)
            {
                // Skybox 'clears' the background color.
                m_opengl->Clear( OpenGL::DEPTH_BUFFER_BIT );
                m_opengl->EnableDepthTest(false);
                m_opengl->EnableDepthWrite(false);
                m_skybox->draw(view.data());
                m_opengl->EnableDepthWrite(true);
                m_opengl->EnableDepthTest(true);
            }
            else
            {
                m_opengl->Clear( OpenGL::COLOR_BUFFER_BIT | OpenGL::DEPTH_BUFFER_BIT );
            }

            m_opengl->UseProgram(m_mesh_shader->get_program());

            // Calculate light world position.
            auto light_d = m_camera_axis_length * 3.0f;
            auto light_x = light_d * -std::sin(m_light_rotation_y);
            auto light_z = light_d *  std::cos(m_light_rotation_y);
            Point4d light_position{ light_x, m_view_gl_state.view_projection.get_camera_position()[1], light_z, 10.0f };

            // Upload matrices: v & vp.
            constexpr auto transpose = false;
            m_opengl->UniformMatrix4fv(m_uniform_model_view_matrix, 1, transpose, model_view.data());
            m_opengl->UniformMatrix4fv(m_uniform_model_view_projection_matrix, 1, transpose, mvp.data());

            // Upload light view position.
            m_opengl->Uniform4fv(m_uniform_light_view_position, 1, light_position);

            // Upload debug settings.
            m_opengl->Uniform1i(m_uniform_debug_output, m_debug_output_value);

            if(m_mesh)
            {
                m_mesh->draw();
            }

            return true;
        }

        std::shared_ptr<Screen> Show_model_screen::get_next_screen() const noexcept
        {
            return std::shared_ptr<Screen>();
        }

        Show_model_data* Show_model_screen::get_data()
        {
            return m_game->get_data();
        }

        void Show_model_screen::on_mouse_click_down(const MouseButton button,
                                                    const bool single,
                                                    const int x,
                                                    const int y) noexcept
        {
            if( button == MouseButton::Left and single )
            {
                if( x >= 0 and y >= 0 )
                {
                    m_view_gl_state.mouse_ball.start_drag(x, y);
                }
            }
        }

        void Show_model_screen::on_mouse_move(const int x, const int y) noexcept
        {
            if( m_view_gl_state.mouse_ball.is_dragging() )
            {
                m_view_gl_state.mouse_ball.drag_to(x, y);
            }
        }

        void Show_model_screen::on_mouse_click_up(const MouseButton button,
                                                  const bool single,
                                                  const int x,
                                                  const int y) noexcept
        {
            if( button == MouseButton::Left and single )
            {
                m_view_gl_state.mouse_ball.drag_to(x, y);
                auto rotation = m_view_gl_state.mouse_ball.get_drag_rotation();
                auto current  = m_scene_mat4;
                mul_4x4( rotation.data(), current, m_scene_mat4 );
                m_view_gl_state.mouse_ball.end_drag();
            }
        }

        //----------------------------------------------------------------------

        Show_model_game::Show_model_game(const std::string_view& model_file,
                                         const std::string_view& skybox_file,
                                         const bool debugging,
                                         const std::uint64_t max_milliseconds)
        : m_model_data(std::make_shared<Show_model_data>())
        {
            m_model_data->model_file  = model_file;
            m_model_data->skybox_file = skybox_file;
            m_model_data->debugging   = debugging;
            m_model_data->max_milliseconds = max_milliseconds;
        }

        Show_model_game::~Show_model_game()
        {
        }

        Show_model_data*  Show_model_game::get_data()
        {
            return m_model_data.get();
        }

        std::string Show_model_game::get_application_name() noexcept
        {
            return EveLike_game::AppName;
        }

        std::string Show_model_game::get_organisation_name() noexcept
        {
            return EveLike_game::OrgName;
        }

        std::pair<int,int> Show_model_game::get_gl_version() noexcept
        {
            return std::make_pair(4,6);
        }

        std::string Show_model_game::get_window_title() noexcept
        {
            return get_application_name();
        }

        std::optional<Image> Show_model_game::get_window_icon_image() noexcept
        {
            return evelike::get_evelike_window_icon_image();
        }

        std::shared_ptr<Screen> Show_model_game::get_initial_screen()
        {
            auto s = std::dynamic_pointer_cast<Show_model_game>( get_shared_ptr() );
            return std::make_shared<Show_model_screen>( s );
        }

    } // namespace

    int show_model(const std::string_view& model_file,
                   const std::string_view& skybox_file,
                   const bool debugging,
                   const std::uint64_t max_milliseconds)
    {
        return solosnake::GameGL::run_game<Show_model_game>(model_file,
                                                            skybox_file,
                                                            debugging,
                                                            max_milliseconds);
    }
}
