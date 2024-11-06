#include "solosnake/blue/blue_codeeditor_widgetfactory.hpp"
#include "solosnake/blue/blue_codeeditor_widget.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/external/lua/lunar.hpp"

#include <cassert>

using namespace std;
using namespace solosnake;

namespace blue
{
    codeeditor_widgetfactory::codeeditor_widgetfactory(
        const std::shared_ptr<solosnake::luaguicommandexecutor>& lce,
        const std::shared_ptr<solosnake::widgetrenderingstyles>& styles,
        const std::shared_ptr<compiler>& bcc )
        : solosnake::ilua_widgetfactory( "codeeditor" )
        , lce_( lce )
        , renderingStyles_( styles )
        , compiler_( bcc )
    {
        assert( bcc );

        if( ! lunar<codeeditor_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<codeeditor_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    codeeditor_widgetfactory::~codeeditor_widgetfactory()
    {
    }

    std::shared_ptr<solosnake::iwidget> codeeditor_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // This can throw if any params are invalid or missing.
        const widgetparams params = ixml_widgetfactory::read_widgetparams( xmlWidget );

        // Read optional text size (default is zero) - reuse textbutton_widgetfactory
        // attributes.
        int textsize = 0;
        solosnake::read_attribute( xmlWidget, solosnake::textbutton_widgetfactory::AttributeTextSize, textsize, true );

        const string stylename = ixml_widgetfactory::read_style( xmlWidget );
        auto widgetrender = renderingStyles_->get_widget_rendering_style( stylename );

        const auto fnames = ilua_widgetfactory::read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        return make_shared<codeeditor_widget>(
                   params,
                   compiler_,
                   lce_,
                   widgetrender,
                   fnames,
                   textsize );
    }
}
