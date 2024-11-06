#include <cassert>
#include "solosnake/text_widget.hpp"
#include "solosnake/text_widgetfactory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace std;

namespace solosnake
{
    //! The (optional) text id to use for the button. This is used to lookup the correct
    //! text to display with this button in the translation table.
    const char text_widgetfactory::AttributeTextId[] = "text_id";

    //! The (optional) text size to use. This is not a point size, but a relative
    //! text size, with the default being zero, and 1, 2, 3 etc being larger and
    //! -1, -2 etc being smaller. Only integer values are supported.
    const char text_widgetfactory::AttributeTextSize[] = "textsize";

    text_widgetfactory::text_widgetfactory(
        shared_ptr<luaguicommandexecutor> lce,
        shared_ptr<widgetrenderingstyles> styles,
        shared_ptr<ilanguagetext> language )
        : ilua_widgetfactory( "text" )
        , lce_( lce )
        , renderingStyles_( styles )
        , language_( language )
    {
        if( !lunar<text_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<text_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget>
    text_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // This can throw if some params are missing or out of range.
        const widgetparams params = read_widgetparams( xmlWidget );

        // Read optional UTF-8 "text"
        string textid;
        solosnake::read_attribute( xmlWidget, AttributeTextId, textid, true );

        // read optional text size (default is zero)
        int textsize = 0;
        solosnake::read_attribute( xmlWidget, AttributeTextSize, textsize, true );

        const string stylename = read_style( xmlWidget );
        auto widgetrender      = renderingStyles_->get_widget_rendering_style( stylename );
        auto fnames            = read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        return make_shared<text_widget>( params, lce_, fnames, textid, textsize, widgetrender, language_ );
    }
}
