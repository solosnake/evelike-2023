#include <limits>
#include "solosnake/draginfo.hpp"
#include "solosnake/dragpackage.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ishape.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/xml.hpp"

namespace solosnake
{
    namespace
    {
        //! Call after Lua pcall returns a non-zero. Logs the error and pops the
        //! message from the Lua stack.
        void report_error( lua_State* L, const std::string& functionName )
        {
            ss_err( "Lua error calling '", functionName, "', (", lua_tostring( L, -1 ), ")" );
            lua_pop( L, 1 );
        }

        //! Call the named Lua function, passing it the item at the given
        //! index and the bool.
        void widget_cursorstates_call( const std::string& function_name,
                                       lua_State* L,
                                       const std::string& widget_name,
                                       cursorbuttons_state s )
        {
            lua_getglobal( L, function_name.c_str() ); // function to be called

            // Push 1st arg (self)
            lua_getglobal( L, widget_name.c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push args 2-6
            lua_pushboolean( L, s.is_action_set() );
            lua_pushboolean( L, s.is_middle_set() );
            lua_pushboolean( L, s.is_context_set() );
            lua_pushboolean( L, s.is_ctrl_set() );
            lua_pushboolean( L, s.is_shift_set() );

            // do the call (6 arguments, 0 results)
            if( lua_pcall( L, 6, 0, 0 ) != 0 )
            {
                report_error( L, function_name );
            }
        }

        //! Call the named Lua button press handler.
        void widget_button_pressed_call( const std::string& function_name,
                                         lua_State* L,
                                         const std::string& widget_name,
                                         screenxy xy,
                                         unsigned short button,
                                         bool inside )
        {
            lua_getglobal( L, function_name.c_str() ); // function to be called

            // Push 1st arg (self)
            lua_getglobal( L, widget_name.c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push 2nd arg (button index).
            lua_pushinteger( L, static_cast<lua_Integer>( button ) );

            // Push 3rd arg (inside/outside).
            lua_pushboolean( L, inside );

            // Push 4th / 5th arg (x/y).
            lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_x() ) );
            lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_y() ) );

            // do the call (5 arguments, nresults results)
            if( lua_pcall( L, 5, 0, 0 ) != 0 )
            {
                report_error( L, function_name );
            }
        }

        //! Call the named Lua key press handler.
        //! modifierFlags arg is not used.
        void widget_key_pressed_call( const std::string& function_name,
                                      lua_State* L,
                                      const std::string& widget_name,
                                      unsigned short key,
                                      unsigned int )
        {
            lua_getglobal( L, function_name.c_str() ); // function to be called

            // Push 1st arg (self)
            lua_getglobal( L, widget_name.c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push 2nd arg (key v code).
            lua_pushinteger( L, static_cast<lua_Integer>( key ) );

            // do the call (2 arguments, 0 results)
            if( lua_pcall( L, 2, 0, 0 ) != 0 )
            {
                report_error( L, function_name );
            }
        }

        //! Calls the widgets 'on_init' method.
        void widget_constructor_call( const std::string& function_name,
                                      lua_State* L,
                                      const std::string& widget_name )
        {
            // Get function to be called.
            lua_getglobal( L, function_name.c_str() );

            // Push 1st arg (self)
            lua_getglobal( L, widget_name.c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // do the call (1 arguments, 0 result)
            if( lua_pcall( L, 1, 0, 0 ) != 0 )
            {
                report_error( L, function_name );
            }
        }
    }

    ilua_widget::ilua_widget( const widgetparams& params,
                              const iwidget::KeyboardFocusing kbf,
                              const std::shared_ptr<luaguicommandexecutor>& lce,
                              const ilua_widget::functionnames& functionNames,
                              WidgetDisablePolicy policy,
                              iwidget::SharedPtrOnly )
        : iwidget( params, kbf, iwidget::SharedPtrOnly() )
        , functions_( functionNames )
        , lua_ce_( lce )
    {
        bool disable = false;

        switch( policy )
        {
            case DisableIfNoClickedHandler:
                disable = functionNames.on_clicked_function_name.empty()
                          && functionNames.on_doubleclicked_function_name.empty();
                break;

            case DefaultEnabled:
                disable = false;
                break;

            case DefaultDisabled:
            default:
                disable = true;
        }

        if( disable )
        {
            disable_widget();
        }
        else
        {
            enable_widget();
        }
    }

    ilua_widget::ilua_widget( const widgetparams& params,
                              const iwidget::KeyboardFocusing kbf,
                              std::unique_ptr<ishape> shape,
                              const std::shared_ptr<luaguicommandexecutor>& lce,
                              const ilua_widget::functionnames& functionNames,
                              WidgetDisablePolicy policy,
                              iwidget::SharedPtrOnly )
        : iwidget( params, kbf, std::move( shape ), iwidget::SharedPtrOnly() )
        , functions_( functionNames )
        , lua_ce_( lce )
    {
        bool disable = false;

        switch( policy )
        {
            case DisableIfNoClickedHandler:
                disable = functionNames.on_clicked_function_name.empty()
                          && functionNames.on_doubleclicked_function_name.empty();
                break;

            case DefaultEnabled:
                disable = false;
                break;

            case DefaultDisabled:
            default:
                disable = true;
        }

        if( disable )
        {
            disable_widget();
        }
        else
        {
            enable_widget();
        }
    }

    ilua_widget::~ilua_widget()
    {
    }

    //! Calls the widgets "constructor". This cannot be safely done
    //! automatically by any base class so it is up to the inheriting class to
    //! call this once at the correct time.
    void ilua_widget::call_init()
    {
        // Call the widgets 'constructor' method (on_init).
        if( false == functions_.on_init_name.empty() )
        {
            widget_constructor_call( functions_.on_init_name, lua_ce_->lua(), name() );
        }
    }

    void ilua_widget::on_cursor_enter( screenxy, cursorbuttons_state s )
    {
        if( false == functions_.on_cursor_enter_function_name.empty() )
        {
            widget_cursorstates_call( functions_.on_cursor_enter_function_name, lua(), name().str(), s );
        }
    }

    void ilua_widget::on_cursor_moved( screenxy, cursorbuttons_state s )
    {
        if( false == functions_.on_cursor_moved_function_name.empty() )
        {
            widget_cursorstates_call( functions_.on_cursor_moved_function_name, lua(), name().str(), s );
        }
    }

    void ilua_widget::on_cursor_exit( screenxy, cursorbuttons_state s )
    {
        if( false == functions_.on_cursor_exit_function_name.empty() )
        {
            widget_cursorstates_call( functions_.on_cursor_exit_function_name, lua(), name().str(), s );
        }
    }

    void ilua_widget::call_wheelmove_function( const char* fn, screenxy xy, float wheelDelta )
    {
        auto L = lua();

        lua_getglobal( L, fn ); // function to be called

        // Push 1st arg (self)
        lua_getglobal( L, name().str().c_str() );
        assert( lua_isuserdata( L, -1 ) );

        // Push 2nd arg (delta).
        lua_pushnumber( L, static_cast<lua_Number>( wheelDelta ) );

        // Push (x/y).
        lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_x() ) );
        lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_y() ) );

        // do the call (4 arguments, 0 results)
        if( lua_pcall( L, 4, 0, 0 ) != 0 )
        {
            report_error( L, fn );
        }
    }

    void ilua_widget::on_wheelmoved_inside( screenxy xy, float wheelDelta )
    {
        if( false == functions_.on_wheelmoved_inside_function_name.empty() )
        {
            call_wheelmove_function(
                functions_.on_wheelmoved_inside_function_name.c_str(), xy, wheelDelta );
        }
    }

    void ilua_widget::on_wheelmoved_outside( screenxy xy, float wheelDelta )
    {
        if( false == functions_.on_wheelmoved_outside_function_name.empty() )
        {
            call_wheelmove_function(
                functions_.on_wheelmoved_outside_function_name.c_str(), xy, wheelDelta );
        }
    }

    ButtonPressOutcome ilua_widget::on_button_pressed( bool inside, screenxy xy, unsigned short button )
    {
        if( inside )
        {
            // If it has a click handler, return exclusive.
            ButtonPressOutcome outcome = functions_.on_clicked_function_name.empty()
                                         ? IgnoreButtonPress
                                         : ExclusiveButtonPress;

            if( false == functions_.on_button_pressed_inside_function_name.empty() )
            {
                auto L = lua();

                // function to be called.
                lua_getglobal( L, functions_.on_button_pressed_inside_function_name.c_str() );

                // Push 1st arg (self)
                lua_getglobal( L, name().str().c_str() );
                assert( lua_isuserdata( L, -1 ) );

                // Push 2nd arg (button index).
                lua_pushinteger( L, static_cast<lua_Integer>( button ) );

                // Push (x/y).
                lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_x() ) );
                lua_pushinteger( L, static_cast<lua_Integer>( xy.screen_y() ) );

                // do the call (4 arguments, 0 results)
                if( lua_pcall( L, 4, 0, 0 ) != 0 )
                {
                    report_error( L, functions_.on_button_pressed_inside_function_name );
                }
            }

            // See if it wants to drag drop.
            if( false == functions_.on_get_drag_info_function_name.empty() )
            {
                outcome = BeginDraggingButtonPress;
            }

            return outcome;
        }

        return IgnoreButtonPress;
    }

    ButtonReleaseOutcome ilua_widget::on_button_released( bool inside, screenxy xy, unsigned short button )
    {
        if( inside )
        {
            if( false == functions_.on_button_released_inside_function_name.empty() )
            {
                widget_button_pressed_call( functions_.on_button_released_inside_function_name,
                                            lua(),
                                            name().str(),
                                            xy,
                                            button,
                                            true );
            }
        }
        else
        {
            if( false == functions_.on_button_released_outside_function_name.empty() )
            {
                widget_button_pressed_call( functions_.on_button_released_outside_function_name,
                                            lua(),
                                            name().str(),
                                            xy,
                                            button,
                                            false );
            }
        }

        return ActionCompleted;
    }

    void ilua_widget::on_clicked( unsigned short button, screenxy xy )
    {
        if( false == functions_.on_clicked_function_name.empty() )
        {
            widget_button_pressed_call(
                functions_.on_clicked_function_name, lua(), name().str(), xy, button, true );
        }
    }

    void ilua_widget::on_double_clicked( unsigned short button, screenxy xy )
    {
        if( false == functions_.on_doubleclicked_function_name.empty() )
        {
            widget_button_pressed_call(
                functions_.on_doubleclicked_function_name, lua(), name().str(), xy, button, true );
        }
    }

    void ilua_widget::on_key_pressed( unsigned short key, unsigned int modifierFlags )
    {
        if( false == functions_.on_key_pressed_function_name.empty() )
        {
            widget_key_pressed_call(
                functions_.on_key_pressed_function_name, lua(), name().str(), key, modifierFlags );
        }
    }

    void ilua_widget::on_key_released( unsigned short key, unsigned int modifierFlags )
    {
        if( false == functions_.on_key_released_function_name.empty() )
        {
            widget_key_pressed_call(
                functions_.on_key_released_function_name, lua(), name().str(), key, modifierFlags );
        }
    }

    //! Calls the associated lua function, passing in the Lua widget object,
    //! and the new width and height of the resized screen.
    //! If an error is encountered in the call it is logged and
    //! execution continues.
    void ilua_widget::on_screen_resized( const dimension2d<unsigned int>& newWindowSize )
    {
        if( false == functions_.on_screen_resized_function_name.empty() )
        {
            lua_State* const L = lua();

            // Get function to be called.
            lua_getglobal( L, functions_.on_screen_resized_function_name.c_str() );

            // Push 1st arg (self)
            lua_getglobal( L, name().c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push 2nd and 3rd args.
            lua_pushnumber( L, static_cast<lua_Number>( newWindowSize.width() ) );
            lua_pushnumber( L, static_cast<lua_Number>( newWindowSize.height() ) );

            // do the call (3 arguments, 0 results)
            if( lua_pcall( L, 3, 0, 0 ) != 0 )
            {
                report_error( L, functions_.on_screen_resized_function_name );
            }
        }
    }

    void ilua_widget::on_text_received( wchar_t t )
    {
        if( false == functions_.on_text_received_function_name.empty() )
        {
            lua_State* const L = lua();

            // Get function to be called.
            lua_getglobal( L, functions_.on_text_received_function_name.c_str() );

            // Push 1st arg (self)
            lua_getglobal( L, name().c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push 2nd arg (unicode char)
            lua_pushnumber( L, static_cast<lua_Number>( t ) );

            // do the call (2 arguments, 0 result)
            if( lua_pcall( L, 2, 0, 0 ) != 0 )
            {
                report_error( L, functions_.on_text_received_function_name );
            }
        }
    }

    void ilua_widget::on_reset()
    {
        if( false == functions_.on_reset_function_name.empty() )
        {
            lua_State* const L = lua();

            // Get function to be called.
            lua_getglobal( L, functions_.on_reset_function_name.c_str() );

            // Push 1st arg (self)
            lua_getglobal( L, name().c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // do the call (1 arguments, 0 result)
            if( lua_pcall( L, 1, 0, 0 ) != 0 )
            {
                report_error( L, functions_.on_reset_function_name );
            }
        }
    }

    void ilua_widget::on_keyboard_focus_gained()
    {
        // Does nothing by default.
    }

    void ilua_widget::on_keyboard_focus_lost()
    {
        // Does nothing by default.
    }

    void ilua_widget::on_dragged_over_by( const draginfo&, screenxy, cursorbuttons_state )
    {
        // Does nothing by default.
    }

    void ilua_widget::on_drag_drop_receive( std::unique_ptr<dragpackage>, screenxy )
    {
        // Does nothing by default.
    }

    void ilua_widget::advance_one_frame()
    {
        // Does nothing by default.
    }

    std::unique_ptr<draginfo> ilua_widget::on_get_drag_info( screenxy ) const
    {
        // Does nothing by default.
        return std::unique_ptr<draginfo>();
    }

    std::unique_ptr<dragpackage> ilua_widget::on_get_drag_package( const draginfo& ) const
    {
        // Does nothing by default.
        return std::unique_ptr<dragpackage>();
    }
}
