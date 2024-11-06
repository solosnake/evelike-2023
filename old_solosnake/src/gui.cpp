#include <algorithm>
#include <cassert>
#include "solosnake/gui.hpp"
#include "solosnake/guiloader.hpp"
#include "solosnake/iinput_events.hpp"
#include "solosnake/iscreenstate.hpp"
#include "solosnake/ixml_widgetfactory.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    gui::gui( const TiXmlNode& guiXMLNode,
              const dimension2d<unsigned int>& windowSize,
              const shared_ptr<iscreenstate>& ss,
              const shared_ptr<ixml_widgetfactory>& factory )
        : screenstate_( ss )
        , last_moves_xy_( 0, 0 )
        , widget_under_cursor_( nullptr )
        , keyboard_focused_widget_( nullptr )
        , actioned_widget_( nullptr )
        , actioned_widget_is_dragging_( false )
        , application_has_focus_( false )
    {
        guiloader loader( factory );

        if( ixmlelementreader::read( &guiXMLNode, loader ) )
        {
            list<shared_ptr<iwidget>> loadedWidgets;
            loader.move_widgets_to( loadedWidgets );

            if( loadedWidgets.empty() )
            {
                ss_wrn( "No widgets defined in view." );
            }

            iwidgetcollection wc( move( loadedWidgets ) );
            layout_ = layout::make_shared( move( wc ) );
        }
        else
        {
            ss_err( "gui failed to read gui widgets from XML." );
            ss_throw( "gui failed to read widgets from xml." );
        }

        screen_was_resized_to( windowSize );
        set_default_keyboard_focus();
    }

    gui::gui( iwidgetcollection&& widgets,
              const dimension2d<unsigned int>& windowSize,
              const shared_ptr<iscreenstate>& ss )
        : layout_( layout::make_shared( move( widgets ) ) )
        , screenstate_( ss )
        , last_moves_xy_( 0, 0 )
        , widget_under_cursor_( nullptr )
        , keyboard_focused_widget_( nullptr )
        , actioned_widget_( nullptr )
        , actioned_widget_is_dragging_( false )
        , application_has_focus_( false )
    {
        layout_->set_gui( this );
        screen_was_resized_to( windowSize );
        set_default_keyboard_focus();
    }

    void gui::render( const unsigned int dt )
    {
        auto i = layout_->widgets().cbegin();
        const auto end = layout_->widgets().cend();
        while( i != end )
        {
            // Comment this out to render no gui.
            ( *i )->render( dt );
            ++i;
        }
    }

    void gui::tab_keyboard_focus_to_next_widget()
    {
        if( keyboard_focused_widget_ == nullptr )
        {
            set_default_keyboard_focus();
        }
        else
        {
            const int focusedTab = keyboard_focused_widget_->tab_order_value();
            int lowestAfterCurrent = numeric_limits<int>::max();
            iwidget* nextwidget = nullptr;

            // Find the set of widgets with the lowest tab values who also
            // accept focus. We don't set the focus yet however.
            for_each( layout_->widgets().cbegin(),
                      layout_->widgets().cend(),
                      [&]( const shared_ptr<iwidget>& w )
            {
                if( w->accepts_keyboard_focus() && w.get() != keyboard_focused_widget_ )
                {
                    const int wtab = w->tab_order_value();
                    if( wtab > focusedTab && wtab < lowestAfterCurrent )
                    {
                        lowestAfterCurrent = wtab;
                        nextwidget = w.get();
                    }
                }
            } );

            // If we didn't find a next highest, restart at the lowest.
            if( nextwidget == nullptr )
            {
                set_default_keyboard_focus();
            }
            else if( nextwidget != keyboard_focused_widget_ )
            {
                set_keyboard_focused_widget( nextwidget );
            }
        }
    }

    void gui::screen_was_resized_to( const dimension2d<unsigned int>& windowSize )
    {
        layout_->screen_was_resized( windowSize );
    }

    // We *can* received focus gained messages without having a focus lost (in
    // debugging at least).
    // Handle this case here. Reset to safe state.
    void gui::handle_application_focus_change()
    {
        widget_under_cursor_ = nullptr;
        actioned_widget_ = nullptr;
        actioned_widget_is_dragging_ = false;
        layout_->handle_application_focus_change();
    }

    // Application has lost focus. Mouse input etc will be lost and the
    // location of the mouse and its state lost.
    void gui::application_focus_lost()
    {
        application_has_focus_ = false;
        handle_application_focus_change();
    }

    void gui::application_focus_gained()
    {
        handle_application_focus_change();
        application_has_focus_ = true;
    }

    // The focus is set by default to the widget with the lowest tab order.
    void gui::set_default_keyboard_focus()
    {
        int lowestTab = numeric_limits<int>::max();
        iwidget* lowestTabbedWidget = nullptr;

        // Find the set of widgets with the lowest tab values who also
        // accept focus. We don't set the focus yet however.
        for_each( layout_->widgets().cbegin(),
                  layout_->widgets().cend(),
                  [&]( const shared_ptr<iwidget>& w )
        {
            if( w->accepts_keyboard_focus() )
            {
                const int wtab = w->tab_order_value();
                if( wtab < lowestTab )
                {
                    // We found a new lower layer.
                    lowestTabbedWidget = w.get();
                    lowestTab = wtab;
                }
            }
        } );

        if( lowestTabbedWidget && ( lowestTabbedWidget != keyboard_focused_widget_ ) )
        {
            set_keyboard_focused_widget( lowestTabbedWidget );
        }
    }

    // This should be the only call that sets to keyboard focused widget.
    void gui::set_keyboard_focused_widget( iwidget* const w )
    {
        if( keyboard_focused_widget_ != w )
        {
            if( keyboard_focused_widget_ )
            {
                assert( keyboard_focused_widget_->has_keyboard_focus() );
                keyboard_focused_widget_->keyboard_focus_lost();
                assert( false == keyboard_focused_widget_->has_keyboard_focus() );
                keyboard_focused_widget_ = nullptr;
            }

            keyboard_focused_widget_ = w;

            if( keyboard_focused_widget_ )
            {
                keyboard_focused_widget_->keyboard_focus_gained();
                assert( keyboard_focused_widget_->accepts_keyboard_focus() );
                assert( keyboard_focused_widget_->has_keyboard_focus() );
            }
        }
    }

    // Called by the layout when this widget changes position.
    void gui::on_widget_changed_location( iwidget& )
    {
    }

    // Called by the layout when this widget changes enabled state.
    void gui::on_widget_changed_enabled( const iwidget& w )
    {
        if( &w == keyboard_focused_widget_ )
        {
            if( w.is_widget_disabled() )
            {
                set_keyboard_focused_widget( nullptr );
            }
        }

        if( &w == actioned_widget_ )
        {
            if( w.is_widget_disabled() )
            {
                actioned_widget_->reset();
                actioned_widget_ = nullptr;
                actioned_widget_is_dragging_ = false;
            }
        }
    }

    // Processes the array of events. Stops the loop if the screen-state
    // says the screen is ended after all events are processed.
    LoopResult gui::process_inputs( const iinput_events& e )
    {
        const std::vector<input_event>& events = e.events();
        const size_t n = e.events().size();

        for( size_t i = 0u; i < n; ++i )
        {
            process_input_event( events[i] );
        }

        return screenstate_->is_screen_ended() ? StopLooping : LoopAgain;
    }

    void gui::advance_one_frame()
    {
        auto i = layout_->widgets().begin();
        const auto end = layout_->widgets().end();
        while( i != end )
        {
            ( *i )->advance_one_frame();
            ++i;
        }
    }

    // Processes a single event. When the application does not have focus, it
    // just consumes events, watching for a focus_gained event.
    void gui::process_input_event( const input_event& evnt )
    {
        if( application_has_focus_ )
        {
            switch( evnt.event_type() )
            {
                case input_event::focus_lost_event:
                    application_focus_lost();
                    break;

                // We shouldn't be able to get a gained event while we already
                // have focus:
                case input_event::focus_gained_event:
                    // assert( ! "Shouldn't be able to receive this message." );
                    application_focus_gained();
                    break;

                case input_event::cursor_moved_event:
                    process_moved_event( evnt.cursor(), evnt.cursor_move_xy() );
                    break;

                case input_event::button_down_event:
                    process_button_down_event( evnt.cursor(), evnt.cursor_move_xy() );
                    break;

                case input_event::button_up_event:
                    process_button_up_event( evnt.cursor(), evnt.cursor_move_xy() );
                    break;

                case input_event::button_dbl_clicked_event:
                    process_doubleclick_event( evnt.cursor(), evnt.cursor_move_xy() );
                    break;

                case input_event::key_down_event:
                case input_event::key_up_event:
                    process_keypress_event(
                        evnt.event_type() == static_cast<uint8_t>( input_event::key_down_event ),
                        evnt.vkey(),
                        evnt.key_modifiers() );
                    break;

                case input_event::text_event:
                    process_text_event( evnt.text_character() );
                    break;

                case input_event::wheel_moved:
                    process_wheelmoved_event( evnt.wheeldelta(), evnt.cursor_move_xy() );
                    break;

                default:
                    break;
            }
        }
        else
        {
            switch( evnt.event_type() )
            {
                case input_event::focus_gained_event:
                    application_focus_gained();
                    break;

                default:
                    break;
            }
        }
    }

    // Handles text input. Text is only sent to the keyboard focused widget.
    void gui::process_text_event( const wchar_t txt )
    {
        if( keyboard_focused_widget_ )
        {
            keyboard_focused_widget_->text_received( txt );
        }
    }

    // Tells the keyboard focused widget (if there is one) that a
    // key press occurred.
    void gui::process_keypress_event( const bool isKeyDown,
                                      const unsigned short vkeyCode,
                                      const unsigned int keyModifiers )
    {
        if( keyboard_focused_widget_ )
        {
            if( isKeyDown )
            {
                keyboard_focused_widget_->key_pressed( vkeyCode, keyModifiers );
            }
            else
            {
                keyboard_focused_widget_->key_released( vkeyCode, keyModifiers );
            }
        }
    }

    void gui::process_moved_event( const cursorbuttons_state cbs, const screenxy xy )
    {
        iwidget* const previouslyUnderCursor = widget_under_cursor_;
        last_moves_cbs_ = cbs;
        last_moves_xy_  = xy;
        widget_under_cursor_ = layout_->highest_widget_under( xy.screen_x(), xy.screen_y() );

        // If we have an actioned widget, it is as if no other widgets existed:
        if( actioned_widget_ )
        {
            if( previouslyUnderCursor != widget_under_cursor_ )
            {
                if( previouslyUnderCursor == actioned_widget_ )
                {
                    actioned_widget_->cursor_exited( xy, cbs );
                }

                if( widget_under_cursor_ == actioned_widget_ )
                {
                    actioned_widget_->cursor_entered( xy, cbs );
                }
            }

            // Actioned widget ALWAYS receives move information.
            actioned_widget_->cursor_moved( xy, cbs );

            if( actioned_widget_is_dragging_ && actioned_widget_draginfo_ && widget_under_cursor_ )
            {
                // A widget CAN drag drop to itself.
                widget_under_cursor_->dragged_over_by( *actioned_widget_draginfo_, xy, cbs );
            }
        }
        else
        {
            // No actioned widgets - inform each widget we move across
            // that we are entering/exiting their active areas.
            if( previouslyUnderCursor != widget_under_cursor_ )
            {
                if( previouslyUnderCursor )
                {
                    previouslyUnderCursor->cursor_exited( xy, cbs );
                }

                if( widget_under_cursor_ )
                {
                    widget_under_cursor_->cursor_entered( xy, cbs );
                }
            }

            if( widget_under_cursor_ )
            {
                widget_under_cursor_->cursor_moved( xy, cbs );
            }
        }
    }

    void gui::process_wheelmoved_event( const short delta, const screenxy xy )
    {
        const float step = static_cast<float>( delta ) / SS_WHEEL_STEPSIZE;
        assert( delta );

        // We need to assume these are accurate:

        if( actioned_widget_ )
        {
            // When we have an actioned widget, it is as if there are no other
            // widgets on the screen.
            if( actioned_widget_->contains( xy ) )
            {
                actioned_widget_->button_wheelmoved_inside( xy, step );
            }
            else
            {
                actioned_widget_->button_wheelmoved_outside( xy, step );
            }
        }
        else if( widget_under_cursor_ )
        {
            widget_under_cursor_->button_wheelmoved_inside( xy, step );
        }
    }

    // Finds the widget under the cursor and calls the correct down event on it.
    void gui::process_button_down_event( const cursorbuttons_state cbs, const screenxy xy )
    {
        // If we have no widget over information, fake a widget moved event.
        // This will set up the mouse over state correctly.
        // Also check that the down event has not moved to another part of the
        // screen without a move event happening.
        if( widget_under_cursor_ == nullptr || ( last_moves_cbs_ != cbs || last_moves_xy_ != xy ) )
        {
            process_moved_event( cbs, xy );
        }

        if( actioned_widget_ )
        {
            // When we have an actioned widget, it is as if there are no other
            // widgets on the screen.
            if( actioned_widget_->contains( xy ) )
            {
                actioned_widget_->button_pressed_inside( xy, cbs );
            }
            else
            {
                actioned_widget_->button_pressed_outside( xy, cbs );
            }
        }
        else if( widget_under_cursor_ )
        {
            const auto outcome = widget_under_cursor_->button_pressed_inside( xy, cbs );

            if( outcome != IgnoreButtonPress )
            {
                actioned_widget_ = widget_under_cursor_;
                if( outcome == BeginDraggingButtonPress )
                {
                    actioned_widget_is_dragging_ = true;
                    actioned_widget_draginfo_ = actioned_widget_->get_drag_info( xy );
                }
            }

            // Clicking on an enabled, focusable widget gives it keyboard focus.
            if( widget_under_cursor_->accepts_keyboard_focus() )
            {
                set_keyboard_focused_widget( widget_under_cursor_ );
            }
        }
    }

    // Finds the widget under the cursor and calls the correct up event on it.
    void gui::process_button_up_event( const cursorbuttons_state cbs, const screenxy xy )
    {
        // If we have no widget over information, fake a widget moved event.
        // This will set up the mouse over state correctly.
        // Also check that the up event has not moved to another part of the
        // screen without a move event happening.
        if( widget_under_cursor_ == nullptr || ( last_moves_cbs_ != cbs || last_moves_xy_ != xy ) )
        {
            process_moved_event( cbs, xy );
        }

        if( actioned_widget_ )
        {
            const bool releaseInside = actioned_widget_->contains( xy );

            const ButtonReleaseOutcome outcome = releaseInside
                                                 ? actioned_widget_->button_released_inside( xy, cbs )
                                                 : actioned_widget_->button_released_outside( xy, cbs );

            if( outcome == ActionCompleted )
            {
                actioned_widget_ = nullptr;
            }
        }
        else if( widget_under_cursor_ )
        {
            assert( widget_under_cursor_->contains( xy ) );
            widget_under_cursor_->button_released_inside( xy, cbs );
        }
    }

    // Calls the appropriate handler on the widget(s).
    void gui::process_doubleclick_event( const cursorbuttons_state cursorButtonsState,
                                         const screenxy xy )
    {
        if( iwidget* const xyWidget = layout_->highest_widget_under( xy.screen_x(), xy.screen_y() ) )
        {
            xyWidget->double_clicked( cursorstate( xy, cursorButtonsState ) );
        }
    }
}
