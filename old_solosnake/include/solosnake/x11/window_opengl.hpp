#ifndef solosnake_x11_window_opengl_hpp
#define solosnake_x11_window_opengl_hpp

#include "solosnake/ioswindow.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <utility>
#include <GL/glx.h>

namespace solosnake
{
    class X11Window;

    struct x11windowopenglparams
    {
        x11windowopenglparams();
        // Defaults:
        size_t width;                      // 800
        size_t height;                     // 600
        size_t zDepth;                     // 24
        size_t stencil;                    // 8
        std::pair<int, int> openGlVersion; // 3.3
        bool allowOldOpenGL;               // false (Core only when false)
        bool enableDebugging;              // false
    };

    class x11windowopengl : public ioswindow
    {
    public:

        x11windowopengl(
            const char* title,
            const x11windowopenglparams& = x11windowopenglparams() );

        virtual ~x11windowopengl();

    private:

        bool is_shutdown() const override;

        void swap_buffers() override;

        void show() override;

        bool check_messages() override;

        void set_as_fullscreen( const bool ) override;

        void set_window_title( const std::string& ) override;

        bool is_fullscreen() const override;

        bool enable_drag_and_drop( bool ) override;

        bool drag_and_drop_enabled() const override;

        rect get_window_rect() const override;

        rect get_screen_rect() const override;

        const void* get_window_handle() const override;

        void handle_size_change( unsigned int, unsigned int );

    protected:

        void on_attached( const std::shared_ptr<iwindow>&  oldwindow,
                          const std::shared_ptr<iwindow>& newwindow ) override;

    private:

        x11windowopengl( const x11windowopengl& );
        x11windowopengl& operator=( const x11windowopengl& );

    private:
        std::unique_ptr<X11Window>  window_;
        GLXContext                  context_;
        unsigned int                windowWidth_;
        unsigned int                windowHeight_;
        std::uint32_t               alt_ctrl_shift_;
        std::uint32_t               mouse_button_released_id_;
        std::uint64_t               mouse_release_time_;
        bool                        continue_update_;
    };
}

#endif
