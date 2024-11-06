#include "solosnake/blue/blue_actionbar_widgetfactory.hpp"
#include "solosnake/blue/blue_codeinspector_widgetfactory.hpp"
#include "solosnake/blue/blue_codeeditor_widgetfactory.hpp"
#include "solosnake/blue/blue_deck.hpp"
#include "solosnake/blue/blue_gamebackground_widgetsfactory.hpp"
#include "solosnake/blue/blue_gamebackground_widgetfactory.hpp"
#include "solosnake/blue/blue_instructionbutton_widgetfactory.hpp"
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/background_widgetfactory.hpp"
#include "solosnake/checkbutton_widgetfactory.hpp"
#include "solosnake/hexbutton_widgetfactory.hpp"
#include "solosnake/picturebutton_widgetfactory.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/text_widgetfactory.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    gamebackground_widgetsfactory::gamebackground_widgetsfactory(
        const weak_ptr<igameview>& gv,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& widgetRenderStyles,
        const shared_ptr<ifilefinder>& stylesPaths,
        const shared_ptr<ilanguagetext>& language,
        const shared_ptr<deck>& playersdeck,
        const shared_ptr<pendinginstructions>& pender,
        const shared_ptr<compiler>& bcc )
        : widgetsfactory( lce )
    {
        register_widget_factory(
            make_shared<text_widgetfactory>(lce, widgetRenderStyles, language));

        register_widget_factory(
            make_shared<textbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory(
            make_shared<checkbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory(
            make_shared<picturebutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory(
            make_shared<hexbutton_widgetfactory>( lce, widgetRenderStyles, language ) );

        register_widget_factory(
            make_shared<gamebackground_widgetfactory>(
                gv,
                pender,
                lce,
                widgetRenderStyles,
                playersdeck->deck_owner() ) );

        register_widget_factory(
            make_shared<instructionbuttonwidgetfactory>(
                static_pointer_cast<ipendinginstructions>( pender ),
                lce,
                widgetRenderStyles,
                language,
                bcc ) );

        register_widget_factory(
            make_shared<codeinspector_widgetfactory>(
                lce,
                widgetRenderStyles,
                gv,
                playersdeck->deck_owner(),
                bcc ) );

        register_widget_factory(
            make_shared<codeeditor_widgetfactory>(
                lce,
                widgetRenderStyles, 
                bcc ) );

        register_widget_factory(
            make_shared<actionbar_widgetfactory>( 
                lce,
                widgetRenderStyles, 
                stylesPaths, 
                playersdeck ) );
    }

    gamebackground_widgetsfactory::~gamebackground_widgetsfactory()
    {
    }
}
