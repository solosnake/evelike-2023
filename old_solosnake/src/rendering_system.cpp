#include "solosnake/rendering_system.hpp"

using namespace std;

namespace solosnake
{
    shared_ptr<rendering_system> rendering_system::create( shared_ptr<window> w,
                                                           deferred_renderer::Quality quality,
                                                           shared_ptr<ifilefinder> modelspaths,
                                                           shared_ptr<ifilefinder> fontspaths,
                                                           shared_ptr<imesh_cache> mc,
                                                           shared_ptr<iimg_cache> ic,
                                                           shared_ptr<iskybox_cache> sbc )
    {
        auto rs = make_shared<rendering_system>( w, quality, modelspaths, fontspaths, mc, ic, sbc, HeapOnly() );

        rendering_system* prs = rs.get();

        auto callbackfn = [ = ]( const windowchange & e, const window & w )
        {
            if( e.event_type() == windowchange::ResizeChange )
            {
                prs->handle_screensize_changed(
                    *reinterpret_cast<const dimension2d<unsigned int>*>( e.event_data() ), w );
            }
        };

        rs->window_events_ = make_shared<callback<windowchange, window>>( callbackfn );
        w->add_observer( rs->window_events_ );

        return rs;
    }

    rendering_system::rendering_system( std::shared_ptr<window> w,
                                        deferred_renderer::Quality quality,
                                        shared_ptr<ifilefinder> modelspaths,
                                        shared_ptr<ifilefinder> fontspaths,
                                        shared_ptr<imesh_cache> mc,
                                        shared_ptr<iimg_cache> ic,
                                        shared_ptr<iskybox_cache> sbc,
                                        const HeapOnly& )
        : window_( w )
        , window_rect_cached_( w->get_window_rect() )
        , screen_rect_cached_( w->get_screen_rect() )
        , window_dimensions_cached_( w->get_window_dimensions() )
    {
        auto r = make_shared<deferred_renderer>(
                     screen_rect_cached_.width(),
                     screen_rect_cached_.height(),
                     quality );

        r->set_viewport( 0, 0, window_rect_cached_.width(), window_rect_cached_.height() );

        renderer_ = make_shared<renderer_cache>( modelspaths, fontspaths, mc, ic, sbc, r );
    }

    rendering_system::~rendering_system()
    {
        // The order of shutdown IS important - we need the renderer dead
        // BEFORE it's window. Although the order is correct in the object
        // declaration, we make it explicit here anyways.
        renderer_.reset();
        window_.reset();
    }

    void rendering_system::handle_screensize_changed( const dimension2d<unsigned int>& newSize,
                                                      const window& w )
    {
        window_rect_cached_ = w.get_window_rect();
        screen_rect_cached_ = w.get_screen_rect();
        window_dimensions_cached_ = newSize;
        renderer_->get_renderer().set_viewport( 0, 0, newSize.width(), newSize.height() );
    }

    void rendering_system::swap_buffers()
    {
        window_->swap_buffers();
    }
}
