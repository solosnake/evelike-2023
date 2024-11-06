#include "solosnake/throw.hpp"
#include "solosnake/background_widget.hpp"
#include "solosnake/background_widgetfactory.hpp"
#include "solosnake/iwidgetname.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace std;

namespace solosnake
{
    background_widgetfactory::background_widgetfactory(
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles )
        : ilua_widgetfactory( "background" )
        , lce_( lce )
        , renderingStyles_( styles )
    {
        if( false == lunar<background_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<background_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget> background_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        const iwidgetname name = ixml_widgetfactory::read_name( xmlWidget );
        const string stylename = ixml_widgetfactory::read_style( xmlWidget );
        const auto renderer    = renderingStyles_->get_widget_rendering_style( stylename );
        const auto fnames      = ilua_widgetfactory::read_and_register_functions( lce_->lua(), name, xmlWidget );

        return background_widget::make_background_widget( name, lce_, renderer, fnames );
    }
}
