#ifndef SOLOSNAKE_EVELIKE_SCREEN_UNIVERSE_HPP
#define SOLOSNAKE_EVELIKE_SCREEN_UNIVERSE_HPP

#include <array>
#include <memory>
#include "solosnake/evelike-screen.hpp"
#include "solosnake/evelike-game.hpp"
#include "solosnake/shadergl.hpp"

namespace solosnake::evelike
{
    class Universe_screen final : public Evelike_screen
    {
    public:

        explicit Universe_screen(std::shared_ptr<EveLike_game>);

        ~Universe_screen() noexcept;

        void on_window_size_change(unsigned int width,
                                   unsigned int height) noexcept final;

        void on_keydown(std::int32_t, std::uint16_t ) noexcept final;

        void on_mouse_move(int x, int y) noexcept final;

        void on_mouse_wheel(float horizontal, float vertical) noexcept final;

        void on_mouse_click_down(MouseButton, bool single, int x, int y) noexcept final;

        void on_mouse_click_up(MouseButton, bool single, int x, int y) noexcept final;

        void on_quit() noexcept final;

        /// Called prior to screen render/update usage.
        void prepare() final;

        /// When this returns true the system swaps the back buffer.
        bool render() final;

        Action update( std::uint64_t elapsed_ms ) final;

        std::shared_ptr<Screen> get_next_screen() const noexcept final;

    private:

        void load_universe_graphics();

        void unload_universe_graphics() noexcept;

        void sort_solar_system_indices(const float* mvp);

    private:

        std::unique_ptr<ShaderGL>               m_shader;
        std::vector<std::uint16_t>              m_depth_sorted_indices;
        std::vector<Point3d>                    m_solar_system_centres;
        std::vector<float>                      m_solar_system_screen_z;
        unsigned int                            m_universe_vao;
        std::array<unsigned int,2>              m_universe_vbo;
        unsigned int                            m_uniform_mvp_mat4;
        unsigned int                            m_uniform_height_of_near_plane;
        OpenGL*                                 m_opengl;
        mutable std::unique_ptr<Screen>         m_next_screen;
        std::array<float,16>                    m_scene_transform;
        float                                   m_rotation_y { 0.0f };
        float                                   m_rotation_rate { 0.16f };
        bool                                    m_quit;
    };
}

#endif
