#include <cassert>
#include <memory>
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_datapathfinder.hpp"
#include "solosnake/blue/blue_load_skybox.hpp"
#include "solosnake/blue/blue_skyboxbackground_widget.hpp"
#include "solosnake/blue/blue_skyboxbackground_widgetfactory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    skyboxbackground_widgetfactory::skyboxbackground_widgetfactory(
        const shared_ptr<datapaths>& paths,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles )
        : ilua_widgetfactory( "skyboxbackground" )
        , datapaths_( paths )
        , lce_( lce )
        , renderingStyles_( styles )
    {
        assert( lce );
        assert( styles );

        if( false == lunar<skyboxbackground_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<skyboxbackground_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget>
    skyboxbackground_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        auto name       = ixml_widgetfactory::read_name( xmlWidget );
        auto stylename  = ixml_widgetfactory::read_style( xmlWidget );
        auto wgtrndr    = renderingStyles_->get_widget_rendering_style( stylename );
        auto renderer   = wgtrndr->renderer_ptr();
        auto skyboxname = read_attribute( xmlWidget, "skyboxfile", false );

        // If rotaterate is unspecified, the skybox will not rotate.
        float rotaterate = 0.0f;
        read_attribute( xmlWidget, "rotaterate", rotaterate, true );

        float axis[3]  = { 0.0f };
        read_attribute( xmlWidget, "axisx", axis[0], false );
        read_attribute( xmlWidget, "axisy", axis[1], false );
        read_attribute( xmlWidget, "axisz", axis[2], false );

        auto skybox    = load_skybox( skyboxname, datapaths_ );
        auto visible   = true;

        auto fnames = ilua_widgetfactory::read_and_register_functions( lce_->lua(), name, xmlWidget );

        return make_shared<skyboxbackground_widget>( name,
                                                     skybox,
                                                     visible,
                                                     axis[0],
                                                     axis[1],
                                                     axis[2],
                                                     rotaterate,
                                                     lce_,
                                                     renderer,
                                                     fnames );
    }
}
