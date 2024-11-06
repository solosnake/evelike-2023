#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_codeinspector_widget.hpp"
#include "solosnake/blue/blue_codeinspector_widgetfactory.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    codeinspector_widgetfactory::codeinspector_widgetfactory(
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles,
        const weak_ptr<igameview>& v,
        const shared_ptr<player>& localplayer,
        const shared_ptr<compiler>& bcc )
        : ilua_widgetfactory( "codeinspector" )
        , lce_( lce )
        , renderingStyles_( styles )
        , player_( localplayer )
        , compiler_( bcc )
        , gameview_( v )
    {
        assert( ! v.expired() );
        assert( localplayer );
        assert( bcc );

        if( ! lunar<codeinspector_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<codeinspector_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget> codeinspector_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
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

        // Throws if weak pointer has expired.
        shared_ptr<igameview> gview( gameview_ );

        return make_shared<codeinspector_widget>(
                   params,
                   player_,
                   compiler_,
                   gview,
                   lce_,
                   fnames,
                   textsize,
                   widgetrender );
    }
}
