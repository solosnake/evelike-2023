#ifndef solosnake_ilua_widget_hpp
#define solosnake_ilua_widget_hpp

#include "solosnake/iwidget.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/external/lua.hpp"

class TiXmlElement;

namespace solosnake
{
    //! Builds the standardised names and variable names for the calls
    //! of the widget. These are names of Lua functions that will have
    //! been already created and made available in the Lua execution
    //! environment.
    //! Although this class does the bulk of the work associated with a
    //! lua widget, specialisation is still needed for the rendering.
    //! By default the Lua widgets do not receive focus.
    //! Remember: this is NOT a Lunar object. The Lua runtime has no notion
    //! of each ilua_widget widget - instead a widget is a collection of
    //! functions. However lua widgets that expose functionality to Lua may
    //! use Lunar. Inheriting classes should always call the 'call_init'
    //! during or after construction.
    class ilua_widget : public iwidget
    {
    public:

        //! The names of the Lua functions called by the ilua_widget.
        //! These names may be empty, which indicates there is no
        //! Lua function registered for the corresponding method.
        struct functionnames
        {
            std::string on_cursor_enter_function_name;
            std::string on_cursor_moved_function_name;
            std::string on_cursor_exit_function_name;
            std::string on_clicked_function_name;
            std::string on_get_drag_info_function_name;
            std::string on_get_drag_package_function_name;
            std::string on_drag_drop_receive_function_name;
            std::string on_doubleclicked_function_name;
            std::string on_button_pressed_inside_function_name;
            std::string on_button_released_outside_function_name;
            std::string on_button_released_inside_function_name;
            std::string on_wheelmoved_inside_function_name;
            std::string on_wheelmoved_outside_function_name;
            std::string on_key_pressed_function_name;
            std::string on_key_released_function_name;
            std::string on_text_received_function_name;
            std::string on_screen_resized_function_name;
            std::string on_reset_function_name;
            std::string on_init_name;
        };

        virtual ~ilua_widget();

        void on_cursor_enter( screenxy, cursorbuttons_state ) override;

        void on_cursor_moved( screenxy, cursorbuttons_state ) override;

        void on_cursor_exit( screenxy, cursorbuttons_state ) override;

        void on_wheelmoved_inside( screenxy, float wheelDelta ) override;

        void on_wheelmoved_outside( screenxy, float wheelDelta ) override;

        void on_clicked( unsigned short button, screenxy ) override;

        void on_double_clicked( unsigned short b, screenxy ) override;

        ButtonPressOutcome on_button_pressed( bool inside, screenxy, unsigned short button ) override;

        ButtonReleaseOutcome on_button_released( bool inside, screenxy, unsigned short button ) override;

        void on_key_pressed( unsigned short k, unsigned int modifierFlags ) override;

        void on_key_released( unsigned short k, unsigned int modifierFlags ) override;

        void on_text_received( wchar_t ) override;

        void on_screen_resized( const Dimension2d<unsigned int>& windowSize ) override;

        void on_reset() override;

        void on_keyboard_focus_gained() override;

        void on_keyboard_focus_lost() override;

        void on_dragged_over_by( const draginfo&, screenxy, cursorbuttons_state ) override;

        void on_drag_drop_receive( std::unique_ptr<dragpackage>, screenxy ) override;

        void advance_one_frame() override;

        std::unique_ptr<draginfo> on_get_drag_info( screenxy ) const override;

        std::unique_ptr<dragpackage> on_get_drag_package( const draginfo& ) const override;

    protected:

        enum WidgetDisablePolicy
        {
            DisableIfNoClickedHandler, // Widget will be disabled if on_clicked_function_name is empty
            DefaultEnabled,            // Widget is initially enabled.
            DefaultDisabled            // Widget is initially disabled.
        };

        ilua_widget( const widgetparams&,
                     const iwidget::KeyboardFocusing,
                     const std::shared_ptr<luaguicommandexecutor>&,
                     const functionnames& functionNames,
                     WidgetDisablePolicy,
                     iwidget::SharedPtrOnly );

        ilua_widget( const widgetparams&,
                     const iwidget::KeyboardFocusing,
                     std::unique_ptr<ishape>,
                     const std::shared_ptr<luaguicommandexecutor>&,
                     const functionnames& functionNames,
                     WidgetDisablePolicy,
                     iwidget::SharedPtrOnly );

        lua_State* lua()
        {
            return lua_ce_->lua();
        }

        void call_init();

    private:

        void call_wheelmove_function( const char*, screenxy, float );

    private:

        functionnames                          functions_;
        std::shared_ptr<luaguicommandexecutor> lua_ce_;
    };
}

#endif
