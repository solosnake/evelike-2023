#ifndef solosnake_WIN32OPENGLWINDOW_HPP
#define solosnake_WIN32OPENGLWINDOW_HPP

#include <memory>
#include "solosnake/win32/includewindows.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/noexcept.hpp"

namespace solosnake
{
    //! Add an ICON resource to your application and name it
    //! "ID_SOLOSNAKE_WINDOW_ICON" and it will appear as the window's icon.
    class windowswindowopengl : public ioswindow
    {
    public:

        struct params
        {
            std::string         title;
            unsigned int        width;
            unsigned int        height;
            std::pair<int, int> openGlVersion;
            bool                allowDeprecatedOpenGL;
            bool                enableDebugging;
            bool                fullscreencursor;
            bool                fullscreen;
            bool                allowResizing;
            unsigned int        zDepth;
            unsigned int        fsaa;
            std::string         classname;
        };

        explicit windowswindowopengl( const windowswindowopengl::params& );

        ~windowswindowopengl() SS_NOEXCEPT;

    private:

        bool        is_shutdown() const override;
        void        swap_buffers() override;
        void        show() override;
        bool        is_fullscreen() const override;
        bool        check_messages() override;
        void        set_as_fullscreen( const bool ) override;
        void        set_window_title( const std::string& ) override;
        bool        drag_and_drop_enabled() const override;
        bool        enable_drag_and_drop( const bool ) override;
        rect        get_window_rect() const override;
        rect        get_screen_rect() const override;
        const void* get_window_handle() const override;
        void        on_attached( const std::shared_ptr<iwindow>&, 
                                 const std::shared_ptr<iwindow>& ) override;

    private:

        windowswindowopengl( const windowswindowopengl& ) = delete;

        windowswindowopengl& operator=( const windowswindowopengl& ) = delete;

    private:

        void post_close_msg();

        void minimise();

        void minimised();

        void unminimise();

        void unminimised();

        bool is_minimised() const    {   return minimized_; }

        void open_window( const windowswindowopengl::params& );

        void create_and_show_window( const windowswindowopengl::params& );

        void register_window_class( const std::wstring& classname, const WNDPROC wndProc );

        void handle_possible_size_change();

        void handle_drag_drop( const HDROP ) SS_NOEXCEPT;

        LRESULT msg_proc( HWND, UINT, WPARAM, LPARAM );

        DWORD dwstyle( const bool windowed ) const;
              
        DWORD dwexstyle( const bool windowed ) const;

        DWORD windowed_dwstyle() const;

        DWORD windowed_dwexstyle() const;
        
        DWORD fullscreen_dwstyle() const;

        DWORD fullscreen_dwexstyle() const;

        RECT  fullscreen_rect() const;

        static LRESULT CALLBACK wndproc( HWND, UINT, WPARAM, LPARAM );

    private:

        class windowhandle;

        std::wstring                        active_wnd_classname_;
        std::wstring                        tmp_wnd_classname_;
        HINSTANCE                           hInstance_;
        HCURSOR                             hCursor_;
        std::unique_ptr<windowhandle>       winhandle_;
        int                                 showparam_;
        dimension2dui                       screenSize_;
        std::uint32_t                       alt_ctrl_shift_;
        LONG                                windowedX_;
        LONG                                windowedY_;
        LONG                                windowedWidth_;
        LONG                                windowedHeight_;
        DWORD                               windowedStyle_;
        DWORD                               windowedExStyle_;
        bool                                clipCursorWhenFullscreen_;
        bool                                showCursorWhenFullscreen_;
        bool                                windowed_;
        bool                                allowResizing_;
        bool                                minimized_;
        bool                                maximized_;
        bool                                continue_update_;
        bool                                dragDropOn_;
    };
}

#endif
