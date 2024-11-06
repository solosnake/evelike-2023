#include <sstream>
#include <cstdarg>
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/iluawidget_make_function.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"

// These are the names of the XML atttributes we associate with widgets:
#define ILUAWIDGET_ON_SCREEN_RESIZED "on_screen_resized"

namespace solosnake
{
    //! Ctor just passes on name to single widget factory.
    ilua_widgetfactory::ilua_widgetfactory( const std::string& typeName ) 
        : singlexml_widgetfactory( typeName )
    {
    }

    //! Only available to inheriting Lua widget factories, this call reads the
    //! expected xml definitions of the Lua handlers/callbacks for a widget,
    //! assigns them the correct names depending on the given widget's name,
    //! registers them with the Lua state, a returns their function names.
    //! Throws if there is any problems.
    ilua_widget::functionnames 
    ilua_widgetfactory::read_and_register_functions( lua_State* L, 
                                                     const iwidgetname& widgetname, 
                                                     const TiXmlElement& xmlWidget )
    {
        ilua_widget::functionnames fnames;

        fnames.on_screen_resized_function_name =
            make_function( L, widgetname, "on_screen_resized", xmlWidget, 3, "self", "width", "height" );

        fnames.on_cursor_enter_function_name =
            make_function( L, widgetname, "on_cursor_entered", xmlWidget, 6, "self", "action", "middle", "context", "ctrl", "shift" );

        fnames.on_cursor_exit_function_name =
            make_function( L, widgetname, "on_cursor_exited", xmlWidget, 6, "self", "action", "middle", "context", "ctrl", "shift" );

        fnames.on_button_pressed_inside_function_name =
            make_function( L, widgetname, "on_button_pressed_inside", xmlWidget, 4, "self", "button", "x", "y" );

        fnames.on_button_released_inside_function_name =
            make_function( L, widgetname, "on_button_released_inside", xmlWidget, 4, "self", "button", "x", "y" );

        fnames.on_button_released_outside_function_name =
            make_function( L, widgetname, "on_button_released_outside", xmlWidget, 4, "self", "button", "x", "y" );

        fnames.on_wheelmoved_inside_function_name =
            make_function( L, widgetname, "on_wheelmoved_inside", xmlWidget, 4, "self", "delta", "x", "y" );

        fnames.on_wheelmoved_outside_function_name =
            make_function( L, widgetname, "on_wheelmoved_outside", xmlWidget, 4, "self", "delta", "x", "y" );

        fnames.on_clicked_function_name =
            make_function( L, widgetname, "on_clicked", xmlWidget, 4, "self", "button", "x", "y" );

        fnames.on_doubleclicked_function_name =
            make_function( L, widgetname, "on_double_clicked", xmlWidget, 4, "self", "button", "x", "y" );

        fnames.on_key_pressed_function_name =
            make_function( L, widgetname, "on_key_pressed", xmlWidget, 2, "self", "key" );

        fnames.on_key_released_function_name =
            make_function( L, widgetname, "on_key_released", xmlWidget, 2, "self", "key" );

        fnames.on_text_received_function_name =
            make_function( L, widgetname, "on_text_received", xmlWidget, 2, "self", "wchar" );

        fnames.on_reset_function_name =
            make_function( L, widgetname, "on_rese", xmlWidget, 1, "self" );

        fnames.on_init_name =
            make_function( L, widgetname, "on_init", xmlWidget, 1, "self" );

        return fnames;
    }
}
