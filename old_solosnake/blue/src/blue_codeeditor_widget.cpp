#include "solosnake/blue/blue_codeeditor_widget.hpp"
#include "solosnake/alignment.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/position.hpp"
#include "solosnake/utf8text.hpp"
#include <cassert>

using namespace std;
using namespace solosnake;

namespace blue
{
    LUNAR_CLASS_FUNCTIONS( codeeditor_widget ) =
    {
        LUNAR_FUNCTION( codeeditor_widget, set_screen_quad ),
        LUNAR_FUNCTION_END
    };

    //! This method is never called.
    codeeditor_widget::codeeditor_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , renderer_()
        , editor_()
        , visual_state_( LooksActivatedAndHasKeyboardFocus )
        , textsize_( 0 )
    {
        ss_throw( "Calling creation of codeeditor_widget via Lua is not allowed." );
    }

    codeeditor_widget::codeeditor_widget(
        const widgetparams& params,
        const shared_ptr<compiler>& bcc,
        const shared_ptr<luaguicommandexecutor> lce,
        const shared_ptr<iwidgetrenderer>& r,
        const ilua_widget::functionnames& fnames,
        const int textsize )
        : ilua_widget( params,
                       iwidget::YesKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , renderer_( r )
        , editor_( make_unique<codeeditor>( bcc ) )
        , visual_state_( LooksActivatedAndHasKeyboardFocus )
        , textsize_( textsize )
    {
        assert( renderer_ );
        lunar<codeeditor_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );
        call_init();
    }

    void codeeditor_widget::render( const unsigned long dt ) const
    {
        solosnake::utf8text name( "TEST" );

        const auto startletterpos = position2df( 0.0f, 0.0f );

        const auto lastletterpos = renderer_->render_text( name,
                                                           textsize_,
                                                           startletterpos,
                                                           dimension2df(),
                                                           HorizontalAlignLeft,
                                                           VerticalAlignTop,
                                                           visual_state_ );

        // Start below the
        auto nextletterpos = position2df( startletterpos.x(), lastletterpos.second.y() );

        renderer_->render_text( editor_->text(),
                                textsize_,
                                nextletterpos,
                                dimension2df(),
                                HorizontalAlignLeft,
                                VerticalAlignTop,
                                visual_state_ );
    }
    
    int codeeditor_widget::set_screen_quad( lua_State* L )
    {
        solosnake::rect r( static_cast<int>( luaL_checknumber( L, 1 ) ),
                           static_cast<int>( luaL_checknumber( L, 2 ) ),
                           dimension2dui( static_cast<unsigned int>( luaL_checknumber( L, 3 ) ),
                                          static_cast<unsigned int>( luaL_checknumber( L, 4 ) ) ) );

        this->set_activearea_rect( r );

        return 0;
    }

    void codeeditor_widget::on_text_received( wchar_t c )
    {
        editor_->on_text_received( c );
    }

    void codeeditor_widget::on_key_pressed( unsigned short k, unsigned int modifierFlags )
    {
        editor_->on_key_pressed( k, modifierFlags );
    }

    void codeeditor_widget::on_key_released( unsigned short k, unsigned int modifierFlags )
    {
        editor_->on_key_released( k, modifierFlags );
    }
}
