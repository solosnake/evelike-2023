#include <cassert>
#include "solosnake/blue/blue_gamebackground_widget.hpp"
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/blue/blue_igameview.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/lua/lunar.hpp"
#include "solosnake/logging.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
#   define SOLOSNAKE_GAMEBACKGROUNDWIDGET_Z        (-1)
#   define SOLOSNAKE_GAMEBACKGROUNDWIDGET_TABORDER (-1)

    //! Initially creates a widget with size 1 x 1 but this is resized to
    //! correct screen size by the required "on_screen_resized" call of the
    //! widget system. The true and the -1 z depth ensures we get focus by
    //! default.
    gamebackground_widget::gamebackground_widget(
        const string& name,
        const weak_ptr<igameview>& gv,
        const shared_ptr<player>& pplayer,
        const shared_ptr<pendinginstructions>& pender,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<iwidgetrenderer>& renderer,
        const ilua_widget::functionnames& fnames )
        : gameviewing_widget( 
            widgetparams( name, 
                          rect( 0, 0, dimension2dui( 1, 1 ) ),
                          SOLOSNAKE_GAMEBACKGROUNDWIDGET_Z,
                          SOLOSNAKE_GAMEBACKGROUNDWIDGET_TABORDER ),
            iwidget::YesKeyBoardFocus,
            ilua_widget::DefaultEnabled,
            gv,
            lce,
            renderer,
            fnames )
        , pendinginstructions_( pender )
        , player_( pplayer )
    {
        assert( player_ );
        assert( ! gv.expired() );
        lunar<gamebackground_widget>::add_as_global( lce->lua(), this, name.c_str(), false );
        call_init();
    }

    LUNAR_CLASS_FUNCTIONS( gamebackground_widget ) =
    {
        LUNAR_FUNCTION_END
    };

    void gamebackground_widget::render( const unsigned long ) const
    {
    }

    //! This method is never called.
    gamebackground_widget::gamebackground_widget( lua_State* )
        : gameviewing_widget()
        , pendinginstructions_()
        , player_()
    {
        ss_throw( "Calling creation of gamebackground_widget via Lua is not allowed." );
    }

    void gamebackground_widget::on_clicked_on_board( const hexcoord& xy )
    {
        send_pending_instructions_to_board( xy );
    }

    void gamebackground_widget::send_pending_instructions_to_board( hexcoord xy )
    {
        // Check to see if we have instructions to deliver:
        shared_ptr<pendinginstructions> pending = pendinginstructions_.lock();
        if( pending && pending->has_instructions() )
        {
            auto code = pending->take_instructions();
            player_->send_instructions_to_board( xy, code );
            std::dynamic_pointer_cast<igameview>( board_view() )->instructions_sent_highlight( xy );
        }
    }
}
