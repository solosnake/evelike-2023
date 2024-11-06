#include "solosnake/background_widgetfactory.hpp"
#include "solosnake/checkbutton_widgetfactory.hpp"
#include "solosnake/hexbutton_widgetfactory.hpp"
#include "solosnake/list_widgetfactory.hpp"
#include "solosnake/picturebutton_widgetfactory.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/text_widgetfactory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_gamebackground_widgetfactory.hpp"
#include "solosnake/blue/blue_menuwidgetsfactory.hpp"
#include "solosnake/blue/blue_skyboxbackground_widgetfactory.hpp"
#include "solosnake/blue/blue_user_settings.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    menuwidgetsfactory::menuwidgetsfactory(
        const std::shared_ptr<user_settings>& settings,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& widgetRenderStyles,
        const shared_ptr<solosnake::ilanguagetext>& language )
        : widgetsfactory( lce )
    {
        if( !language )
        {
            ss_throw( "menuwidgetsfactory constructed with empty ilanguagetext ptr." );
        }

        if( !widgetRenderStyles )
        {
            ss_throw( "menuwidgetsfactory constructed with empty widgetRenderStyles ptr." );
        }

        auto paths = make_shared<datapaths>( settings->data_dirs() );

        register_widget_factory(
            make_shared<text_widgetfactory>(lce, widgetRenderStyles, language));

        register_widget_factory( 
            make_shared<textbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory( 
            make_shared<checkbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory( 
            make_shared<picturebutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory( 
            make_shared<background_widgetfactory>( lce, widgetRenderStyles ) );

        register_widget_factory( 
            make_shared<hexbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory( 
            make_shared<skyboxbackground_widgetfactory>( paths, lce, widgetRenderStyles ) );

        register_widget_factory( 
            make_shared<list_widgetfactory>( lce, widgetRenderStyles ) );
    }

    menuwidgetsfactory::~menuwidgetsfactory()
    {
    }
}
