#ifndef solosnake_window_hpp
#define solosnake_window_hpp
#include <memory>
#include <string>
#include <vector>
#include "solosnake/dimension.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/iwindow.hpp"
#include "solosnake/minimised.hpp"
#include "solosnake/observer.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/windowchange.hpp"
#include "solosnake/windowinputs.hpp"
#include "solosnake/windowobserver.hpp"

namespace solosnake
{
    //! A window class with events which can be subscribed to.
    class window : public iwindow
    {
    protected:

        struct HeapOnly
        {
        };

    public:

        static std::shared_ptr<window> make_shared( std::shared_ptr<ioswindow> );

        window( std::shared_ptr<ioswindow>, const HeapOnly& );

        virtual ~window();

        void show();

        void set_title( const std::string& utf8 );

        const iinput_events& get_new_events();

        const iinput_events& get_events() const;

        Rect get_window_rect() const;

        Rect get_screen_rect() const;

        Dimension2d<unsigned int> get_window_dimensions() const;

        void swap_buffers();

        void add_observer( std::shared_ptr<windowobserver> );

        void remove_observer( std::shared_ptr<windowobserver> );

    protected:

        void on_fullscreen_change( const FullscreenState ) override;

        void on_minimised() override;

        void on_unminimised() override;

        void on_resized( const Dimension2d<unsigned int>& ) override;

        void on_dragdropped( const std::vector<std::wstring>& ) override;

    private:

        mutable observers<windowchange, window> observers_;
        windowinputs                            window_;
    };
}

#endif
