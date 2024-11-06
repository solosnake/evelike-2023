#include <cassert>
#include "solosnake/colour.hpp"
#include "solosnake/iluawidget_make_function.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/checkbutton_widget.hpp"
#include "solosnake/checkbutton_widgetfactory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/external/lua/lunar.hpp"

using namespace std;

namespace solosnake
{
    //! The (optional) text id to use for the button. This is used to lookup the correct
    //! text to display with this button in the translation table.
    const char checkbutton_widgetfactory::AttributeTextId[] = "text_id";

    //! The (optional) text size to use. This is not a point size, but a relative
    //! text size, with the default being zero, and 1, 2, 3 etc being larger and
    //! -1, -2 etc being smaller. Only integer values are supported.
    const char checkbutton_widgetfactory::AttributeTextSize[] = "textsize";

    //! The (optional) check boolean. This decides if the checkbox is created as checked or
    //! unchecked by default. If this param is missing the widget is created unchecked.
    const char checkbutton_widgetfactory::AttributeChecked[] = "checked";

    checkbutton_widgetfactory::checkbutton_widgetfactory(
        shared_ptr<luaguicommandexecutor> lce,
        shared_ptr<widgetrenderingstyles> styles,
        shared_ptr<ilanguagetext> language )
        : ilua_widgetfactory( "checkbutton" )
        , lce_( lce )
        , renderingStyles_( styles )
        , language_( language )
    {
        if( !lunar<checkbutton_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<checkbutton_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget>
    checkbutton_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // This can throw if some params are missing or out of range.
        const widgetparams params = read_widgetparams( xmlWidget );

        // Read optional "checked" boolean - default is unchecked.
        bool checked = false;
        read_attribute( xmlWidget, AttributeChecked, checked, true );

        // Read optional UTF-8 "text"
        string textid;
        read_attribute( xmlWidget, AttributeTextId, textid, true );

        // Read optional text size (default is zero)
        int textsize = 0;
        read_attribute( xmlWidget, AttributeTextSize, textsize, true );

        auto stylename    = read_style( xmlWidget );
        auto widgetrender = renderingStyles_->get_widget_rendering_style( stylename );
        auto L            = lce_->lua();
        auto fnames       = read_and_register_functions( L, params.name, xmlWidget );

        // Register an optional Lua defined function to call when the check is changed.
        auto oncheckchanged_fname =
            make_function( L, params.name, "on_check_changed", xmlWidget, 2, "self", "checked" );

        return make_shared<checkbutton_widget>( 
            params, lce_, fnames, oncheckchanged_fname, checked, textid, textsize, widgetrender, language_ );
    }
}
