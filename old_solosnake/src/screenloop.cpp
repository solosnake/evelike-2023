#include "solosnake/screenloop.hpp"
#include "solosnake/window.hpp"
#include "solosnake/rect.hpp"

using namespace std;

namespace solosnake
{
    screenloop::screenloop( const shared_ptr<window>& w, const HeapOnly& )
        : window_( w )
    {
    }

    screenloop::~screenloop()
    {
    }

    shared_ptr<screenloop>
    screenloop::make_shared( const shared_ptr<window>& w )
    {
        auto s = std::make_shared<screenloop>( w, HeapOnly() );
        auto wo = static_pointer_cast<observer<windowchange, window>>( s );
        w->add_observer( wo );
        return s;
    }

    void screenloop::initial_preframe_update()
    {
    }

    LoopResult screenloop::preframe_update( const unsigned long )
    {
        return LoopAgain;
    }

    LoopResult screenloop::initial_frame_update()
    {
        window_->show();
        return do_one_screenloop( 0u );
    }

    LoopResult screenloop::frame_update( const unsigned long dt )
    {
        return do_one_screenloop( dt );
    }

    LoopResult screenloop::do_one_screenloop( const unsigned long dt )
    {
        const iinput_events& events = window_->get_new_events();

        render_screen( dt );

        return ( update_loop( events ) && !events.is_shutdown() ) ? LoopAgain : StopLooping;
    }

    void screenloop::render_screen( const unsigned long dt )
    {
        start_scene( dt );
        render_scene( dt );
        end_scene( dt );
    }

    LoopResult screenloop::update_loop( const iinput_events& )
    {
        return StopLooping;
    }

    void screenloop::final_frame_update( const unsigned long )
    {
    }

    void screenloop::start_scene( const unsigned long )
    {
    }

    void screenloop::render_scene( const unsigned long )
    {
    }

    void screenloop::end_scene( const unsigned long )
    {
    }

    void screenloop::on_window_resized( const dimension2d<unsigned int>& )
    {
    }

    void screenloop::on_window_fullscreen_changed( FullscreenState )
    {
    }

    void screenloop::on_window_minimised_changed( MinimisedState )
    {
    }

    void screenloop::handle_screensize_changed( const dimension2d<unsigned int>& newSize, const window& )
    {
        this->on_window_resized( newSize );
    }

    void screenloop::handle_fullscreen_changed( const FullscreenState& fs, const window& )
    {
        this->on_window_fullscreen_changed( fs );
    }

    void screenloop::handle_minimised_changed( const MinimisedState& mini, const window& )
    {
        this->on_window_minimised_changed( mini );
    }

    void screenloop::handle_dragdrops( const std::vector<std::wstring>&, const window& )
    {
    }

    void screenloop::on_event( const windowchange& e, const window& w )
    {
        switch( e.event_type() )
        {
            case windowchange::FullscreenChange:
                handle_fullscreen_changed( *e.fullscreenstate(), w );
                break;

            case windowchange::MinimisedChange:
                handle_minimised_changed( Minimised, w );
                break;

            case windowchange::UnminimisedChange:
                handle_minimised_changed( NotMinimised, w );
                break;

            case windowchange::ResizeChange:
                handle_screensize_changed( *e.resized_dimensions(), w );
                break;

            case windowchange::DragDroppedChange:
                handle_dragdrops( *e.drag_drops(), w );
                break;

            default:
                break;
        }
    }
}