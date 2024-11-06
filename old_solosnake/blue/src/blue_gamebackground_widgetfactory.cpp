#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_gamebackground_widget.hpp"
#include "solosnake/blue/blue_gamebackground_widgetfactory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    gamebackground_widgetfactory::gamebackground_widgetfactory(
        const weak_ptr<igameview>& v,
        const shared_ptr<pendinginstructions>& pender,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles,
        const shared_ptr<player>& localplayer )
        : ilua_widgetfactory( "boardbackground" )
        , pendinginstructions_( pender )
        , lce_( lce )
        , renderingStyles_( styles )
        , player_( localplayer )
        , gameview_( v )
    {
        assert( localplayer );

        if( false == lunar<gamebackground_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<gamebackground_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget>
    gamebackground_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        iwidgetname name = ixml_widgetfactory::read_name( xmlWidget );

        string stylename = ixml_widgetfactory::read_style( xmlWidget );

        auto renderer = renderingStyles_->get_widget_rendering_style( stylename );

        // Throws if weak pointer has expired.
        shared_ptr<igameview> gview( gameview_ );

        return make_shared<gamebackground_widget>(
                   name,
                   gview,
                   player_,
                   pendinginstructions_,
                   lce_,
                   renderer,
                   ilua_widgetfactory::read_and_register_functions( lce_->lua(), name, xmlWidget ) );
    }
}
