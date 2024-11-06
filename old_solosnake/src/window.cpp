#include "solosnake/window.hpp"
#include "solosnake/minimised.hpp"

namespace solosnake
{
    std::shared_ptr<window> window::make_shared( std::shared_ptr<ioswindow> iosw )
    {
        std::shared_ptr<window> w = std::make_shared<window>( iosw, HeapOnly() );
        w->attach_to( iosw );
        return w;
    }

    window::window( std::shared_ptr<ioswindow> w, const window::HeapOnly& ) : window_( w )
    {
    }

    window::~window()
    {
        detach();
    }

    void window::show()
    {
        window_.window()->show();
    }

    void window::set_title( const std::string& utf8 )
    {
        window_.window()->set_window_title( utf8 );
    }

    const iinput_events& window::get_new_events()
    {
        window_.clear_events();
        oswindow()->check_messages();
        return window_;
    }

    const iinput_events& window::get_events() const
    {
        return window_;
    }

    rect window::get_window_rect() const
    {
        return oswindow()->get_window_rect();
    }

    rect window::get_screen_rect() const
    {
        return oswindow()->get_screen_rect();
    }

    dimension2d<unsigned int> window::get_window_dimensions() const
    {
        rect r = get_window_rect();
        return dimension2d<unsigned int>( r.width(), r.height() );
    }

    void window::swap_buffers()
    {
        return oswindow()->swap_buffers();
    }

    void window::on_fullscreen_change( const FullscreenState fs )
    {
        observers_.notify_of_event( windowchange( fs ), *this );
    }

    void window::on_minimised()
    {
        observers_.notify_of_event( windowchange( NotMinimised ), *this );
    }

    void window::on_unminimised()
    {
        observers_.notify_of_event( windowchange( Minimised ), *this );
    }

    void window::on_resized( const dimension2d<unsigned int>& newSize )
    {
        observers_.notify_of_event( windowchange( newSize ), *this );
    }

    void window::on_dragdropped( const std::vector<std::wstring>& dropped_files )
    {
        observers_.notify_of_event( windowchange( dropped_files ), *this );
    }

    void window::add_observer( std::shared_ptr<observer<windowchange, window>> p )
    {
        observers_.add_observer( p );
    }

    void window::remove_observer( std::shared_ptr<observer<windowchange, window>> p )
    {
        observers_.remove_observer( p );
    }
}