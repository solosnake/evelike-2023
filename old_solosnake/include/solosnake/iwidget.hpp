#ifndef solosnake_iwidget_hpp
#define solosnake_iwidget_hpp

#include <memory>
#include <string>
#include "solosnake/buttonpressoutcome.hpp"
#include "solosnake/buttonreleaseoutcome.hpp"
#include "solosnake/cursorstate.hpp"
#include "solosnake/draginfo.hpp"
#include "solosnake/dragpackage.hpp"
#include "solosnake/iwidgetname.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/widgetparams.hpp"
#include "solosnake/widgetstate.hpp"

#define WIDGET_IS_DISABLED_FLAG             (0x01u)
#define WIDGET_ACCEPTS_KEYBOARD_FOCUS_FLAG  (0x02u)
#define WIDGET_HAS_KEYBOARD_FOCUS_FLAG      (0x04u)
#define WIDGET_MOUSE_IS_INSIDE_FLAG         (0x08u)
#define WIDGET_ACTION_IS_PRESSED            (0x10u)

namespace solosnake
{
    class layout;
    class ishape;

    //! Base class for GUI elements (widgets).
    //! The widget is enabled by default.
    //! The Rect is the rectangular bounding area of the widget in screen
    //! coordinates - within it is the internal shape of the widget, which
    //! might be non rectangular and which is defined inside a normalised
    //! coordinate system (0,0) to (1,1). When the screen x,y lies inside
    //! the screen active area of the widget, then the screen x,y is
    //! transformed to a normalised point (scaled by the size of the
    //! active area to be a point within (0,0) to (1,1) and the ishape is
    //! queried if this point lies inside it.
    class iwidget : public std::enable_shared_from_this<iwidget>
    {
        friend class layout;
        friend class gui;

    public:

        enum KeyboardFocusing { NoKeyBoardFocus, YesKeyBoardFocus };

        virtual ~iwidget();

        const iwidgetname& name() const;

        bool contains( screenxy xy ) const;

        //! The screen x and y coordinates range from 0,0 (top left) to
        //! w,h (bottom right).
        bool contains(
            int screenx,
            int screeny ) const;

        WidgetState activation_state() const;

        bool is_widget_disabled() const;

        bool is_widget_enabled() const;

        //! Returns true if widget flag says action is pressed.
        bool is_action_pressed() const;

        //! True if the mouse is currently inside the widget.
        bool is_mouse_inside_widget() const;

        //! Returns true if the widget currently has keyboard focus.
        bool has_keyboard_focus() const;

        //! The widgets gui depth - used to determine whether a gui
        //! receives notifications or not, depending on whether there
        //! are widgets with z layers higher than it's over it.
        int zlayer() const;

        //! The tab order value is used to determine which widget keyboard
        //! and input focus shifts to when selecting a next widget in the tab
        //! order. When two or more widgets have the same tab order value, the
        //! one selected depends on its position in the layouts container.
        //! A widget is skipped if it is not enabled.
        int tab_order_value() const;

        //! Returns true if this widget accepts having keyboard focus.
        bool accepts_keyboard_focus() const;

        //! The active area is specified in pixels.
        void set_activearea_rect( const Rect& r );

        //! Sets the widget's z layer. This determines how widgets interact
        //! when one overlays another.
        void set_zlayer( const int z );

        //! Enable the widget.
        void enable_widget();

        //! Disables the widget. Removes keyboard focus etc.
        void disable_widget();

        //! Called by the layout when the keyboard focus is set to this widget.
        //! Calls on_keyboard_focus_gained.
        void keyboard_focus_gained();

        //! Called by the layout when the keyboard focus is lost by this widget.
        //! Calls on_keyboard_focus_lost.
        void keyboard_focus_lost();

        //! The dt is the milliseconds passed since the last call to render. If zero is passed
        //! then the widget should re-render the previous render state (if any).
        virtual void render( const unsigned long dt ) const = 0;

        //! Called every frame on all widgets, to allow widgets to update any
        //! internal state that may change on a per-frame basis.
        virtual void advance_one_frame() = 0;

        //! Called by the GUI when the cursor enters the the active area of the
        //! widget. The widget will not receive messages for every time the
        //! cursor passed over it - when another widget is set as the exclusive
        //! widget the GUI acts as if there were no other widgets on the layout.
        virtual void on_cursor_enter( screenxy, cursorbuttons_state ) = 0;

        //! Called by the GUI when the cursor moves either over this widget or
        //! anywhere if the widget is the exclusive widget. The widget will not
        //! receive messages for every time the cursor passed over it - when
        //! another widget is set as the exclusive widget the GUI acts as if
        //! there were no other widgets on the layout.
        virtual void on_cursor_moved( screenxy, cursorbuttons_state ) = 0;

        //! Called by the GUI when the cursor exits the the active area of the
        //! widget. The widget will not receive messages for every time the
        //! cursor passed over it - when another widget is set as the exclusive
        //! widget the GUI acts as if there were no other widgets on the layout.
        virtual void on_cursor_exit( screenxy, cursorbuttons_state ) = 0;

        //! Called when the mouse is over the widget and the mouse wheel is moved
        //! Called with the mouse XY and the number of whole wheel steps made -
        //! partial steps from higher precision mice may generate steps of less
        //! than 1.0. Positive deltas indicate the user rolled the wheel away
        //! from themselves, negative indicate it was pulled towards them.
        virtual void on_wheelmoved_inside( screenxy, float delta ) = 0;

        //! Called when the widget is active and the mouse wheel is moved
        //! while not over the widget.
        //! Called with the mouse XY and the number of whole wheel steps made -
        //! partial steps from higher precision mice may generate steps of less
        //! than 1.0. Positive deltas indicate the user rolled the wheel away
        //! from themselves, negative indicate it was pulled towards them.
        virtual void on_wheelmoved_outside( screenxy, float delta ) = 0;

        //! Called whenever a mouse button is pressed for this item.
        virtual ButtonPressOutcome on_button_pressed(
            bool inside,
            screenxy,
            unsigned short button ) = 0;

        //! Called whenever a mouse button is pressed for this item.
        virtual ButtonReleaseOutcome on_button_released(
            bool inside,
            screenxy,
            unsigned short button ) = 0;

        //! Called when a click event (down and up inside area) occurs
        //! for the button indicated. 0 is primary/action mouse button,
        //! usually the left mouse button on a right handed mouse,
        //! 1 is secondary mouse button, usually right button on a right
        //! handed mouse, and middle button is 2.
        virtual void on_clicked( unsigned short button, screenxy ) = 0;

        //! Called when a double click event (double clicked down and
        //! up inside area) occurs for the button indicated. 0 is
        //! primary/action mouse button, usually the left mouse button on
        //! a right handed mouse, 1 is secondary mouse button, usually
        //! right button on a right handed mouse, and middle button is 2.
        virtual void on_double_clicked( unsigned short b, screenxy ) = 0;

        //! Called keyboard-as-buttons inputs, k is the Microsoft Windows
        //! virtual key-code are modifierFlags are flags to indicate the state
        //! of the control keys.
        //! Modifier flags are from input_event::modifier_type flags.
        virtual void on_key_pressed( unsigned short k, unsigned int modifierFlags ) = 0;

        //! Called keyboard-as-buttons inputs, k is the Microsoft Windows
        //! virtual key-code are modifierFlags are flags to indicate the state
        //! of the control keys.
        //! Modifier flags are from input_event::modifier_type flags.
        virtual void on_key_released( unsigned short k, unsigned int modifierFlags ) = 0;

        //! For text input: when a handler has keyboard focus. The text
        //! will be sent in character by unicode character.
        virtual void on_text_received( wchar_t ) = 0;

        //! Called when the input handler gains keyboard focus.
        virtual void on_keyboard_focus_gained() = 0;

        //! Called when the input handler loses keyboard focus.
        virtual void on_keyboard_focus_lost() = 0;

        //! Called when the screen is resized with the new width and height,
        //! and also at startup of a window with its initial width and height.
        virtual void on_screen_resized( const Dimension2d<unsigned int>& ) = 0;

        //! Called when the window loses focus. Widgets should respond by
        //! resetting to defaults any states that depends on frame to frame
        //! states etc. This call should not contain any logic as it may be
        //! called several times for the same event. It should simply reset
        //! the widget to a safe unassuming state.
        virtual void on_reset() = 0;

        //! Called when another widget in drag mode moves across this widget.
        //! Allows widget to change display to indicate it can accept drag/drops.
        virtual void on_dragged_over_by(
            const draginfo&,
            screenxy,
            cursorbuttons_state ) = 0;

        //! Called when a widget drag/drops something onto this widget. The
        //! widget is free to do nothing with this call.
        virtual void on_drag_drop_receive(
            std::unique_ptr<dragpackage>,
            screenxy ) = 0;

        //! Called if the widget has returned BeginDraggingButtonPress from a
        //! button_pressed call. This info will be passed to widgets the
        //! cursor moves across while dragging (including the dragging widget
        //! itself).
        virtual std::unique_ptr<draginfo> on_get_drag_info( screenxy ) const = 0;

        //! Called when the GUI is handling this widget as a dragger, and is
        //! about to drop this onto a widget.
        virtual std::unique_ptr<dragpackage> on_get_drag_package( const draginfo& ) const = 0;

    protected:

        struct SharedPtrOnly {};

        iwidget(
            const widgetparams&,
            const KeyboardFocusing,
            const SharedPtrOnly );

        iwidget(
            const widgetparams&,
            const KeyboardFocusing,
            std::unique_ptr<ishape>,
            const SharedPtrOnly );

        const Rect& active_area() const;

    private:

        //! Call to reset any state regarding up/down etc.
        void reset();

        //! Called when the screen has changed size. Also called once initially
        //! to allow the widget to dynamically resize itself. This is called
        //! whether the widget is enabled or not.
        //! Calls on_screen_resized.
        void screen_resized_to( const Dimension2d<unsigned int>& );

        //! Called when a key is pressed and the widget has focus. This should
        //! be used similarly to buttons, and not to read text.
        //! Modifier flags are from input_event::modifier_type flags.
        void key_pressed( const unsigned short key, const unsigned int modifierFlags );

        //! Called when a key is released and the widget has focus. This should
        //! be used similarly to buttons, and not to read text.
        //! Modifier flags are from input_event::modifier_type flags.
        void key_released( const unsigned short key, const unsigned int modifierFlags );

        //! Called when the widget is *double* clicked. This should be used
        //! orthogonally to use of on_clicked as they may interfere or send
        //! double messages for example.
        void double_clicked( cursorstate );

        //! Called when the widget receives a text character (as opposed to a
        //! key-down event). This is the handler to use for string building.
        //! Calls on_text_received.
        void text_received( const wchar_t );

        //! Called when the active area for the iwidget is entered by the cursor.
        void cursor_entered( screenxy, cursorbuttons_state );

        //! Called when the cursor moves and it is of interest to this widget.
        //! The widget can examine its flags to see if the cursor is inside
        //! the active area or not.
        void cursor_moved( screenxy, cursorbuttons_state );

        //! Called when the active area for the iwidget is exited by the cursor.
        void cursor_exited( screenxy, cursorbuttons_state );

        //! Called when another widget in drag mode moves across this widget.
        //! Allows widget to change display to indicate it can accept
        //! drag/drops.
        void dragged_over_by( const draginfo&, screenxy, cursorbuttons_state );

        //! Called when a widget drag/drops something onto this widget. The
        //! widget is free to do nothing with this call.
        void drag_drop_receive( std::unique_ptr<dragpackage>, screenxy );

        //! Called if the widget has returned BeginDraggingButtonPress from a
        //! button_pressed call. This info will be passed to widgets the
        //! cursor moves across while dragging (including the dragging widget
        //! itself).
        std::unique_ptr<draginfo> get_drag_info( screenxy ) const;

        //! Called when the GUI is handling this widget as a dragger, and is
        //! about to drop this onto a widget.
        std::unique_ptr<dragpackage> get_drag_package( const draginfo& ) const;

        //! Called when a button is pressed inside the widget. The result is
        //! used to indicate to the GUI whether it should make this widget
        //! exclusive or not.
        ButtonPressOutcome button_pressed_inside( screenxy, cursorbuttons_state );

        ButtonPressOutcome button_pressed_outside( screenxy, cursorbuttons_state );

        ButtonReleaseOutcome button_released_inside( screenxy, cursorbuttons_state );

        ButtonReleaseOutcome button_released_outside( screenxy, cursorbuttons_state );

        void button_wheelmoved_inside( screenxy, float delta );

        void button_wheelmoved_outside( screenxy, float delta );

        //! The widget may not yet be in the layout at this point, so perform
        //! no interactions with the layout yet.
        void set_layout( std::weak_ptr<layout> );

    private:

        std::weak_ptr<layout>   layout_;
        std::unique_ptr<ishape> shape_;
        iwidgetname             name_;
        Rect                    active_area_;
        int                     zlayer_;
        int                     tab_order_value_;
        char                    flags_;
    };


    //-------------------------------------------------------------------------


    inline void iwidget::set_zlayer( const int z )
    {
        // Z < 0 is reserved for background widget.
        assert( z >= 0 );
        zlayer_ = z;
    }

    inline int iwidget::zlayer() const
    {
        return zlayer_;
    }

    inline const iwidgetname& iwidget::name() const
    {
        return name_;
    }

    inline const Rect& iwidget::active_area() const
    {
        return active_area_;
    }

    inline bool iwidget::is_widget_disabled() const
    {
        return 0 != ( flags_ & WIDGET_IS_DISABLED_FLAG );
    }

    inline bool iwidget::is_widget_enabled() const
    {
        return !is_widget_disabled();
    }

    inline int iwidget::tab_order_value() const
    {
        return tab_order_value_;
    }

    inline bool iwidget::has_keyboard_focus() const
    {
        return 0 != ( flags_ & WIDGET_HAS_KEYBOARD_FOCUS_FLAG );
    }

    inline bool iwidget::accepts_keyboard_focus() const
    {
        return 0 != ( flags_ & WIDGET_ACCEPTS_KEYBOARD_FOCUS_FLAG );
    }

    inline void iwidget::reset()
    {
#ifndef NDEBUG
        const bool enabledBefore = is_widget_enabled();
#endif

        ss_dbg( "All state flags cleared on ", name().c_str() );

        // Keep the enabled/disabled flag and clear all others.
        // Keep keyboard focus too. Only change this when the GUI tells us to.
        // Clear only our own flags.
        flags_ &= ( ( WIDGET_IS_DISABLED_FLAG | WIDGET_ACCEPTS_KEYBOARD_FOCUS_FLAG
                      | WIDGET_HAS_KEYBOARD_FOCUS_FLAG ) & flags_ );

#ifndef NDEBUG
        assert( enabledBefore == is_widget_enabled() );
#endif
        on_reset();
    }

    inline bool iwidget::is_action_pressed() const
    {
        return 0 != ( flags_ & WIDGET_ACTION_IS_PRESSED );
    }

    inline bool iwidget::is_mouse_inside_widget() const
    {
        return 0 != ( flags_ & WIDGET_MOUSE_IS_INSIDE_FLAG );
    }

    inline WidgetState iwidget::activation_state() const
    {
        if( is_widget_enabled() )
        {
            if( is_action_pressed() )
            {
                return is_mouse_inside_widget() ? WidgetActivatedAndCursorInside
                       : WidgetActivatedAndCursorOutside;
            }
            else
            {
                return is_mouse_inside_widget() ? WidgetHighlighted : WidgetNormal;
            }
        }
        else
        {
            return WidgetDisabled;
        }
    }

    inline bool iwidget::contains( screenxy xy ) const
    {
        return contains( xy.screen_x(), xy.screen_y() );
    }
}

#endif
