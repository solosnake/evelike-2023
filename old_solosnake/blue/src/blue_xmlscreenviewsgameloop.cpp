#include <cassert>
#include "solosnake/blue/blue_xmlscreenviewsgameloop.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    xmlscreensviewsgameloop::xmlscreensviewsgameloop( const shared_ptr<window>& wnd,
                                                      const shared_ptr<xmlscreenviews>& views,
                                                      const shared_ptr<rendering_system>& rndr,
                                                      const shared_ptr<sound_renderer>& snds,
                                                      const screenloop::HeapOnly& h )
        : screenloop( wnd, h )
        , views_( views )
        , rendering_( rndr )
        , sounds_( snds )
    {
        assert( views );
        assert( rndr );
        assert( snds );
    }

    shared_ptr<xmlscreensviewsgameloop>
    xmlscreensviewsgameloop::make_shared( const shared_ptr<window>& w,
                                          const shared_ptr<xmlscreenviews>& views,
                                          const shared_ptr<rendering_system>& rndr,
                                          const shared_ptr<sound_renderer>& snds )
    {
        auto s  = std::make_shared<xmlscreensviewsgameloop>( w, views, rndr, snds, screenloop::HeapOnly() );
        xmlscreensviewsgameloop::add_as_observer_of_window( s, w );
        return s;
    }

    shared_ptr<xmlscreensviewsgameloop>
    xmlscreensviewsgameloop::make_shared( const filepath& xmlScreenFile,
                                          const shared_ptr<window>& wndw,
                                          const shared_ptr<rendering_system>& rndr,
                                          const shared_ptr<sound_renderer>& sounds,
                                          const shared_ptr<widgetsfactory>& widgets )
    {
        auto views = xmlscreenviews::make_shared( xmlScreenFile,
                                                  wndw->get_window_dimensions(),
                                                  widgets );

        return xmlscreensviewsgameloop::make_shared( wndw, move( views ), rndr, sounds );
    }

    xmlscreensviewsgameloop::~xmlscreensviewsgameloop()
    {
    }

    void xmlscreensviewsgameloop::add_as_observer_of_window( const shared_ptr<xmlscreensviewsgameloop>& s, 
                                                             const shared_ptr<window>& w )
    {
        auto wo = std::static_pointer_cast<observer<windowchange, window>>( s );
        w->add_observer( wo );
    }

    void xmlscreensviewsgameloop::on_window_resized( const dimension2d<unsigned int>& newSize )
    {
        views_->handle_screensize_changed( newSize );
    }

    void xmlscreensviewsgameloop::on_window_fullscreen_changed( FullscreenState fs )
    {
        views_->handle_fullscreen_changed( fs );
    }

    void xmlscreensviewsgameloop::on_window_minimised_changed( MinimisedState mini )
    {
        views_->handle_minimised_changed( mini );
    }

    void xmlscreensviewsgameloop::start_scene( const unsigned long )
    {
        rendering_->renderer().start_scene();
    }

    void xmlscreensviewsgameloop::render_scene( const unsigned long ms )
    {
        views_->render_active_view( ms );
    }

    void xmlscreensviewsgameloop::end_scene( const unsigned long )
    {
        rendering_->renderer().end_scene();

        if( sounds_ )
        {
            sounds_->update();
        }

        rendering_->swap_buffers();
    }

    LoopResult xmlscreensviewsgameloop::update_loop( const iinput_events& e )
    {
        return views_->handle_inputs( e );
    }
}
