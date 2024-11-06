#include <array>
#include <cassert>
#include <filesystem>
#include <stdexcept>
#include "SDL.h"
#include "solosnake/opengl.hpp"
#include "solosnake/file.hpp"
#include "solosnake/gamegl.hpp"
#include "solosnake/evelike-screen-universe.hpp"

namespace solosnake::evelike
{
    namespace
    {
        /// Returns the 4x4 identity matrix.
        constexpr std::array<float,16> Identity() noexcept
        {
            return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        }
    }

    Universe_screen::Universe_screen(std::shared_ptr<EveLike_game> game)
        : Evelike_screen(game)
        , m_universe_vao(0)
        , m_universe_vbo()
        , m_uniform_mvp_mat4(0)
        , m_uniform_height_of_near_plane(0)
        , m_opengl(game->opengl())
        , m_scene_transform(Identity())
        , m_quit(false)
    {
        assert( game );
        assert( m_opengl );

        if( game->game_data()->universe->get_solar_system_count() > std::numeric_limits<std::uint16_t>::max() )
        {
            throw std::runtime_error("Too many solar systems to draw.");
        }
    }

    Universe_screen::~Universe_screen() noexcept
    {
        unload_universe_graphics();
        m_opengl = nullptr;
    }

    void Universe_screen::unload_universe_graphics() noexcept
    {
        assert(m_opengl);
        // Noexcept operations:
        m_opengl->DeleteVertexArrays(1u, &m_universe_vao);
        m_opengl->DeleteBuffers(static_cast<int>(m_universe_vbo.size()), m_universe_vbo.data());
        m_universe_vao = 0;
        m_universe_vbo.fill(0);
    }

    void Universe_screen::sort_solar_system_indices(const float* mvp)
    {
        // Calculate and store each solar system's screen Z.
        for(auto i=0u; i < m_solar_system_centres.size(); ++i)
        {
            auto centre = m_solar_system_centres[i];
            m_solar_system_screen_z[i] = centre.x * mvp[8]  +
                                         centre.y * mvp[9]  +
                                         centre.z * mvp[10] +
                                         mvp[11];
        }

        // Sort the indices by the screen Z values.
        std::sort( m_depth_sorted_indices.begin(),
                   m_depth_sorted_indices.end(),
                   [this](std::uint16_t lhs, std::uint16_t rhs) -> bool
                    {
                        // Returns ​true if lhs is greater than rhs (so we draw back -> front).
                        return m_solar_system_screen_z[lhs] > m_solar_system_screen_z[rhs];
                    } );
    }

    void Universe_screen::prepare()
    {
        const auto ww = game()->get_window_width();
        const auto wh = game()->get_window_height();

        resize_viewport(ww, wh);
        load_universe_graphics();

        m_opengl->EnableDepthTest(true);
        m_opengl->EnableBackFaceCulling(false);
        m_opengl->EnableLinePolygonMode(true);
        m_opengl->EnableProgramPointSize(true);
        m_opengl->ClearColor(0.078f, 0.139f, 0.35f, 0.0f);
        m_opengl->UseProgram(m_shader->get_program());
        m_opengl->BlendFunc(OpenGL::Blending::SRC_ALPHA, OpenGL::Blending::ONE_MINUS_SRC_ALPHA);

        // Adjust the camera and view settings so we encompass the universe.
        auto universe_radius = game_data()->universe->get_radius();
        Point3d offset{ 0.0f, 0.0f, universe_radius };
        auto camera_look_at  = game_data()->universe->get_centre();
        auto camera_position = camera_look_at + offset;
        const auto slightly_enlarge = 1.025f;

        game_data()->view_gl_state.mouse_ball.end_drag();
        game_data()->view_gl_state.mouse_ball.set_window_size(ww, wh);
        game_data()->view_gl_state.view_projection.set_viewport(ww, wh);
        game_data()->view_gl_state.view_projection.set_z_far( slightly_enlarge * 2.0f * universe_radius );
        game_data()->view_gl_state.view_projection.set_camera_look_at( camera_look_at );
        game_data()->view_gl_state.view_projection.set_camera_position( camera_position );
        game_data()->view_gl_state.view_projection.update();
    }

    Screen::Action Universe_screen::update( std::uint64_t ms )
    {
        constexpr float TwoPi { 2.0f * 3.1415926535898f };

        m_rotation_y += m_rotation_rate * ms * 0.001f;

        while(m_rotation_y > TwoPi)
        {
            m_rotation_y -= TwoPi;
        }

        game_data()->view_gl_state.view_projection.update();

        auto a = m_next_screen ? Screen::Action::ChangeScreen : Screen::Action::Continue;
        return m_quit ? Screen::Action::Quit : a;
    }

    bool Universe_screen::render()
    {
        // Combine current scene transform with the mouseball rotation.
        auto dragged = game_data()->view_gl_state.mouse_ball.get_drag_rotation();
        Matrix4x4 model_matrix;
        mul_4x4( dragged.data(), m_scene_transform.data(), static_cast<float*>(model_matrix) );
        auto mvp = game_data()->view_gl_state.view_projection.get_model_view_projection(model_matrix);

        // Sort the solar system point sprites so we can draw them back to front.
        sort_solar_system_indices(mvp.data());

        // Near plane height is used to draw the solar system sprites with a
        // perspective correct height.
        auto near_plane_height = game_data()->view_gl_state.view_projection.get_near_plane_height();

        m_opengl->Clear(OpenGL::COLOR_BUFFER_BIT | OpenGL::DEPTH_BUFFER_BIT);
        m_opengl->EnableBlending(true);
        m_opengl->EnableDepthTest(false);
        m_opengl->BindVertexArray( m_universe_vao );
        m_opengl->UniformMatrix4fv( m_uniform_mvp_mat4, 1, false, mvp.data() );
        m_opengl->Uniform1f( m_uniform_height_of_near_plane, near_plane_height );
        m_opengl->DrawIndexedPoints( static_cast<unsigned int>(m_depth_sorted_indices.size()),
                                     m_depth_sorted_indices.data() );

        return true;
    }

    void Universe_screen::load_universe_graphics()
    {
        unload_universe_graphics();

        // Prepare index buffer. We can't use iota as it needs signed ints.
        m_depth_sorted_indices.resize( game_data()->universe->get_solar_system_count(), 0u );
        for (auto i = 0u; i < m_depth_sorted_indices.size(); ++i)
        {
            m_depth_sorted_indices[i] = static_cast<std::uint16_t>(i);
        }

        // Pre-allocate solar systems vectors.
        m_solar_system_centres.reserve( game_data()->universe->get_solar_system_count() );
        m_solar_system_screen_z.resize( game_data()->universe->get_solar_system_count(), 0.0f );

        // Check we have no OpenGL errors.
        m_opengl->VerifyNoError("OpenGL error before loading universe graphics.");

        // Each solar system has a centre and radius.
        std::vector<float> solar_system_buffer;
        solar_system_buffer.reserve( game_data()->universe->get_solar_system_count() * 4u );
        // Fill buffer with [x,y,z,r] of each solar system.
        for(auto i=0u; i<game_data()->universe->get_solar_system_count(); ++i)
        {
            auto c = game_data()->universe->get_solar_system_centre(i);
            auto r = game_data()->universe->get_solar_system_radius(i);
            solar_system_buffer.push_back(c.x);
            solar_system_buffer.push_back(c.y);
            solar_system_buffer.push_back(c.z);
            solar_system_buffer.push_back(r);
            // Store each solar system's centre, used later Z sorting.
            m_solar_system_centres.push_back( Point3d{c.x, c.y, c.z} );
        }

        // Create VAO and vertex buffers. Each vertex buffer has details about
        // one solar system. We will use
        m_opengl->GenVertexArrays(1, &m_universe_vao);
        m_opengl->GenBuffers( static_cast<int>(m_universe_vbo.size()), m_universe_vbo.data());

        // Solar system vertex array object.
        m_opengl->BindVertexArray(m_universe_vao);

        // We are not using indices for the solar systems.
        m_opengl->BindBuffer( OpenGL::Target::ELEMENT_ARRAY_BUFFER, 0 );

        // A buffer with the x,y,z and radius.
        m_opengl->BindBuffer(OpenGL::Target::ARRAY_BUFFER, m_universe_vbo[0]);
        m_opengl->BufferData(OpenGL::Target::ARRAY_BUFFER,
                             sizeof(float) * solar_system_buffer.size(),
                             solar_system_buffer.data(),
                             OpenGL::Usage::STATIC_DRAW);
        m_opengl->EnableVertexAttribArray(0);
        m_opengl->VertexAttribPointer(0, 4, OpenGL::DataType::FLOAT, false, 0, nullptr);

        // Unbind to prevent other modifications
        m_opengl->BindVertexArray(0);
        m_opengl->BindBuffer( OpenGL::Target::ARRAY_BUFFER, 0 );

        // Make shader
        auto vert = get_file_contents( shader_file_path("universe.vert") );
        auto frag = get_file_contents( shader_file_path("universe.frag") );
        m_shader  = std::make_unique<ShaderGL>( game()->opengl_ptr(), vert, frag);

        // Get uniform locations.
        m_uniform_mvp_mat4             = m_shader->get_uniform_location("model_view_projection_matrix");
        m_uniform_height_of_near_plane = m_shader->get_uniform_location("height_of_near_plane");

        // Check we didn't cause any OpenGL errors.
        m_opengl->VerifyNoError("OpenGL error after loading universe graphics.");
    }

    void Universe_screen::on_window_size_change(const unsigned int width,
                                                const unsigned int height) noexcept
    {
        resize_viewport(width, height);
    }

    void Universe_screen::on_keydown(const std::int32_t key,
                                     const std::uint16_t ) noexcept
    {
        switch(key)
        {
        case SDLK_q:
            m_quit = true;
            break;

        case SDLK_F11:
            toggle_fullscreen();
            break;

        case SDLK_F12:
            take_screenshot();
            break;

        default:
            break;
        }
    }

    void Universe_screen::on_mouse_wheel(const float,
                                         const float) noexcept
    {
    }

    void Universe_screen::on_mouse_click_down(const MouseButton button,
                                              const bool single,
                                              const int x,
                                              const int y) noexcept
    {
        if( button == MouseButton::Left and single )
        {
            if( x >= 0 and y >= 0 )
            {
                game_data()->view_gl_state.mouse_ball.start_drag(x, y);
            }
        }
    }

    void Universe_screen::on_mouse_move(const int x, const int y) noexcept
    {
        if( game_data()->view_gl_state.mouse_ball.is_dragging() )
        {
            game_data()->view_gl_state.mouse_ball.drag_to(x, y);
        }
    }

    void Universe_screen::on_mouse_click_up(const MouseButton button,
                                            const bool single,
                                            const int x,
                                            const int y) noexcept
    {
        if( button == MouseButton::Left and single )
        {
            game_data()->view_gl_state.mouse_ball.drag_to(x, y);
            auto rotation = game_data()->view_gl_state.mouse_ball.get_drag_rotation();
            auto current  = m_scene_transform;
            mul_4x4( rotation.data(), current.data(), m_scene_transform.data() );
            game_data()->view_gl_state.mouse_ball.end_drag();

        }
    }

    void Universe_screen::on_quit() noexcept
    {
        m_quit = true;
    }

    std::shared_ptr<Screen> Universe_screen::get_next_screen() const noexcept
    {
        return std::shared_ptr<Screen>(m_next_screen.release());
    }
}
