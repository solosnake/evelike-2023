#include <cassert>
#include "solosnake/background_widget.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/lua/lunar.hpp"

using namespace std;

namespace solosnake
{
#   define SOLOSNAKE_BACKGROUNDWIDGET_Z         (-1)
#   define SOLOSNAKE_BACKGROUNDWIDGET_TABORDER  (-1)

    LUNAR_CLASS_FUNCTIONS( background_widget ) = 
    { 
        LUNAR_FUNCTION_END
    };

    background_widget::background_widget(
        const string& name,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<iwidgetrenderer>& renderer,
        const ilua_widget::functionnames& fnames,
        iwidget::SharedPtrOnly spo )
        : ilua_widget( widgetparams( name,
                                     rect( 0, 0, dimension2dui( 1, 1 ) ),
                                     SOLOSNAKE_BACKGROUNDWIDGET_Z,
                                     SOLOSNAKE_BACKGROUNDWIDGET_TABORDER),
                       iwidget::YesKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       spo )
        , renderer_( renderer )
    {
        assert( renderer_ );
        lunar<background_widget>::add_as_global( lce->lua(), this, name.c_str(), false );
        call_init();
    }

    shared_ptr<background_widget> background_widget::make_background_widget(
        const string& name,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<iwidgetrenderer>& renderer,
        const ilua_widget::functionnames& fnames )
    {
        return make_shared<background_widget>( name, lce, renderer, fnames, iwidget::SharedPtrOnly() );
    }

    //! This method is never called.
    background_widget::background_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
    {
        ss_throw( "Calling creation of background_widget via Lua is not allowed." );
    }

    void background_widget::on_screen_resized( const dimension2d<unsigned int>& windowSize )
    {
        this->set_activearea_rect( rect( 0, 0, windowSize ) );
        ilua_widget::on_screen_resized( windowSize );
    }

    void background_widget::render( const unsigned long ) const
    {
    }
}
