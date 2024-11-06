#if 0

#include "solosnake/gui.hpp"
#include "solosnake/guiscreenloop.hpp"
#include "solosnake/guicommandexecutor.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/sound_renderer.hpp"
#include "solosnake/window.hpp"

namespace solosnake
{
    //! The sound renderer is optional and can be nullptr.
    std::shared_ptr<guiscreenloop> guiscreenloop::make_shared( 
            const std::shared_ptr<gui>& g,
            const std::shared_ptr<window>& w,
            const std::shared_ptr<rendering_system>& r,
            const std::shared_ptr<sound_renderer>& s )
    {
        return std::make_shared<guiscreenloop>( g, w, r, s, screenloop::HeapOnly() );
    }

    guiscreenloop::guiscreenloop( const std::shared_ptr<gui>& g,
                                  const std::shared_ptr<window>& w,
                                  const std::shared_ptr<rendering_system>& r,
                                  const std::shared_ptr<sound_renderer>& s,
                                  const screenloop::HeapOnly& h )
        : screenloop( w, h )
        , renderer_( r )
        , sounds_( s )
        , gui_( g )
    {
    }

    void guiscreenloop::render_loop( const unsigned long dt )
    {
        // TODO MOVEINTO GUI 
        renderer_->renderer().start_scene();
        gui_->render( dt );

        if( sounds_ )
        {
            sounds_->update();
        }

        renderer_->renderer().end_scene();
        renderer_->swap_buffers();
    }

    LoopResult guiscreenloop::update_loop( const iinput_events& e )
    {
        return gui_->process_inputs( e );
    }

    void guiscreenloop::on_window_resized( const dimension2d<unsigned int>& windowSize )
    {
        resize_screen( windowSize );
    }

    void guiscreenloop::resize_screen( const dimension2d<unsigned int>& windowSize )
    {
        gui_->screen_was_resized_to( windowSize );
    }
}

#endif
