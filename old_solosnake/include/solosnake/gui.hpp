#ifndef solosnake_gui_hpp
#define solosnake_gui_hpp

#include <memory>
#include <string>
#include <vector>
#include "solosnake/cursorstate.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/iwidget.hpp"
#include "solosnake/loopresult.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/screenxy.hpp"
#include "solosnake/external/xml.hpp"

#define SOLOSNAKE_XML_ELEMENT_GUI "gui"

namespace solosnake
{
    class layout;
    class input_event;
    class iinput_events;
    class iscreenstate;
    class iwidgetcollection;
    class ixml_widgetfactory;

    //! The gui is a collection of gui elements (widgets) loaded from an XML
    //! file. Clicking on an enabled widget or widgets gives them keyboard
    //! focus. When a widget has keyboard focus it receives key and text events.
    //! The background widget typically has -1 z depth and is keyboard focus
    //! enabled, meaning it will by default handle keyboard focused events.
    //! The gui starts in an unfocused state and relies on receiving an initial
    //! focus event to begin focused event processing.
    class gui
    {
        friend class layout;

    public:

        gui( const TiXmlNode&,
             const dimension2d<unsigned int>& windowSize,
             const std::shared_ptr<iscreenstate>&,
             const std::shared_ptr<ixml_widgetfactory>& );

        gui( iwidgetcollection&&,
             const dimension2d<unsigned int>& windowSize,
             const std::shared_ptr<iscreenstate>& );

        //! Renders all the widgets in the GUI.
        //! @dt The milliseconds elapsed since the last call to render. If zero is passed
        //! then the gui should re-render the previous render state (if any).
        void render( const unsigned int dt );

        //! Calls advance_one_frame on all the widgets, allowing them
        //! them to update any internal state that changes per frame, not per
        //! render.
        void advance_one_frame();

        //! Passes the input events to the appropriate widgets, based
        //! on location on-screen.
        LoopResult process_inputs( const iinput_events& );

        //! Call to allow all the widgets to dynamically adjust to new screen sizes.
        void screen_was_resized_to( const dimension2d<unsigned int>& );

        //! Moves the keyboard focus to the next widget in the tab order, and
        //! returns the focused widget, which may be nullptr. The next widget
        //! will be the widget with the next highest tab order value. Widgets
        //! with the same value as the current widget will be skipped.
        void tab_keyboard_focus_to_next_widget();

        //! Sets the keyboard focus to the default widget, the lowest tab order.
        void set_default_keyboard_focus();

    private:

        void on_widget_changed_location( iwidget& );

        void on_widget_changed_enabled( const iwidget& );

        void set_keyboard_focused_widget( iwidget* const );

        void process_input_event( const input_event& );

        void process_text_event( const wchar_t );

        void process_keypress_event( const bool, const unsigned short, const unsigned int );

        void process_button_down_event( const cursorbuttons_state, const screenxy );

        void process_button_up_event( const cursorbuttons_state, const screenxy );

        void process_doubleclick_event( const cursorbuttons_state, const screenxy );

        void process_moved_event( const cursorbuttons_state, const screenxy );

        void process_wheelmoved_event( const short delta, const screenxy );

        void application_focus_gained();

        void application_focus_lost();

        void handle_application_focus_change();

    private:
        
        gui( const gui& ); // = delete
        gui& operator=( const gui& ); // = delete

    private:

        std::shared_ptr<layout>       layout_;
        std::shared_ptr<iscreenstate> screenstate_;
        cursorbuttons_state           last_moves_cbs_;
        screenxy                      last_moves_xy_;
        iwidget*                      widget_under_cursor_;
        iwidget*                      keyboard_focused_widget_;
        iwidget*                      actioned_widget_;
        bool                          actioned_widget_is_dragging_;
        std::unique_ptr<draginfo>     actioned_widget_draginfo_;
        bool                          application_has_focus_;
    };
}

#endif
