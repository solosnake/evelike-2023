#include <cassert>
#include "solosnake/blue/blue_guiscreenview.hpp"
#include "solosnake/logging.hpp"

using namespace solosnake;

namespace blue
{
    guiscreenview::guiscreenview( const std::shared_ptr<solosnake::gui>& g,
                                  const std::string& viewname )
        : iscreenview( viewname )
        , gui_( g )
    {
        fakeEvents_.reserve( 1u );
        assert( g );
    }

    guiscreenview::~guiscreenview()
    {
    }

    void guiscreenview::render_view( const unsigned long dt ) const
    {
        gui_->render( dt );
    }

    void guiscreenview::handle_fullscreen_changed( const FullscreenState& )
    {
    }

    void guiscreenview::handle_minimised_changed( const MinimisedState& )
    {
    }

    void guiscreenview::handle_screensize_changed( const dimension2d<unsigned int>& windowSize )
    {
        gui_->screen_was_resized_to( windowSize );
    }

    LoopResult guiscreenview::handle_inputs( const iinput_events& e )
    {
        return gui_->process_inputs( e );
    }

    void guiscreenview::advance_one_frame()
    {
        gui_->advance_one_frame();
    }

    void guiscreenview::activate_view()
    {
        ss_log( "guiscreenview '", screenview_name(), "' activated." );

        // Send 'fake' focus gained event.
        fakeEvents_.clear();
        fakeEvents_.push_back( input_event::make_focus_gained() );
        gui_->process_inputs( *this );
    }

    void guiscreenview::deactivate_view()
    {
        ss_log( "guiscreenview '", screenview_name(), "' de-activated." );

        // Send 'fake' focus lost event.
        fakeEvents_.clear();
        fakeEvents_.push_back( input_event::make_focus_lost() );
        gui_->process_inputs( *this );
    }

    const std::vector<solosnake::input_event>& guiscreenview::events() const
    {
        assert( !fakeEvents_.empty() );
        return fakeEvents_;
    }

    void guiscreenview::clear_events()
    {
        fakeEvents_.clear();
    }

    bool guiscreenview::is_shutdown() const
    {
        return false;
    }
}
