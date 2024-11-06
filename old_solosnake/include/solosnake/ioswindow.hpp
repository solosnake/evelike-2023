#ifndef solosnake_ioswindow_hpp
#define solosnake_ioswindow_hpp

#include <memory>
#include "solosnake/rect.hpp"
#include "solosnake/inputs.hpp"
#include "solosnake/iwindow.hpp"

namespace solosnake
{
    class ioswindow : public std::enable_shared_from_this<ioswindow>
    {
        friend class iwindow;

    public:

        virtual ~ioswindow();

        //! Returns true if the window is in a closed state and should not be
        //! shown or used.
        virtual bool is_shutdown() const = 0;

        //! Swaps internal back and front buffer(s).
        virtual void swap_buffers() = 0;

        //! Can be called multiple times, displays the window to the user.
        virtual void show() = 0;

        //! If this returns false the internal window should be considered to
        //! be in an closed state and should no longer be used.
        //! This call is required for the window message pump to deliver
        //! messages and it should be called in the main game loop.
        virtual bool check_messages() = 0;

        //! If possible sets the window title bar to this UTF8 string.
        virtual void set_window_title( const std::string& utf8 ) = 0;

        //! Sets window into fullscreen or windowed mode depending on boolean.
        virtual void set_as_fullscreen( const bool ) = 0;

        //! Returns true if window is in fullscreen mode.
        virtual bool is_fullscreen() const = 0;

        //! Attempt to enable drag & drop for window. Returns true if
        //! successfully enabled.
        virtual bool enable_drag_and_drop( const bool ) = 0;

        //! Returns true if drag & drop is enabled for this window.
        virtual bool drag_and_drop_enabled() const = 0;

        virtual Rect get_window_rect() const = 0;

        virtual const void* get_window_handle() const = 0;

        //! Returns the screen rectangle. Left and top are always zero.
        virtual Rect get_screen_rect() const = 0;

        bool is_attached( std::shared_ptr<iwindow> w ) const;

        const inputs& input_changes() const;

        void clear_input_events();

    protected:

        ioswindow();

        iwindow* attached_window();

        const iwindow* attached_window() const;

        inputs& input_changes();

        virtual void on_attached(
            const std::shared_ptr<iwindow>& oldwindow,
            const std::shared_ptr<iwindow>& newwindow ) = 0;

    private:

        void attach( std::shared_ptr<iwindow> );

    private:

        inputs                  input_events_;
        std::weak_ptr<iwindow>  window_;

    public:

        static std::shared_ptr<ioswindow> make_shared(
            const unsigned int width,
            const unsigned int height,
            const bool allowResizing,
            const std::string& title,
            const bool fullscreen,
            const std::string& classname,
            const std::string& options );
    };


    //////////////////////////////////////////////////////////////////////////


    inline bool ioswindow::is_attached( std::shared_ptr<iwindow> w ) const
    {
        return w == window_.lock();
    }

    inline const inputs& ioswindow::input_changes() const
    {
        return input_events_;
    }

    inline void ioswindow::clear_input_events()
    {
        input_events_.clear_events();
    }

    inline iwindow* ioswindow::attached_window()
    {
        return window_.lock().get();
    }

    inline const iwindow* ioswindow::attached_window() const
    {
        return window_.lock().get();
    }

    inline inputs& ioswindow::input_changes()
    {
        return input_events_;
    }
}

#endif
