#include "solosnake/list_widget.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    LUNAR_CLASS_FUNCTIONS( list_widget ) =
    {
        LUNAR_FUNCTION_END
    };

    //! This method is never called.
    list_widget::list_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
    {
        ss_throw( "Calling creation of list_widget via Lua is not allowed." );
    }

    list_widget::list_widget( const shared_ptr<vector<string>>& items,
                              const widgetparams& params,
                              const shared_ptr<luaguicommandexecutor> lce,
                              const ilua_widget::functionnames& fnames,
                              const shared_ptr<iwidgetrenderer> renderingStyle,
                              iwidget::SharedPtrOnly spo )
        : ilua_widget( params,
                       iwidget::NoKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       spo )
        , list_( items )
    {
        assert( renderingStyle );

        lunar<list_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    shared_ptr<list_widget>
    list_widget::make_filelist_widget( const shared_ptr<vector<string>>& items,
                                       const widgetparams& params,
                                       const shared_ptr<luaguicommandexecutor>& lce,
                                       const shared_ptr<iwidgetrenderer>& renderer,
                                       const ilua_widget::functionnames& fnames )
    {
        return make_shared<list_widget>( items,
                                         params,
                                         lce,
                                         fnames,
                                         renderer,
                                         iwidget::SharedPtrOnly() );
    }

    void list_widget::render( const unsigned long dt ) const
    {
    }
}
