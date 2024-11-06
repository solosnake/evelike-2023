#include <cassert>
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/text_widget.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/external/lua/lunar.hpp"

namespace solosnake
{
    LUNAR_CLASS_FUNCTIONS( text_widget ) =
    {
        LUNAR_FUNCTION( text_widget, set_screen_quad ),
        LUNAR_FUNCTION( text_widget, get_screen_quad ),
        LUNAR_FUNCTION( text_widget, set_text_id ),
        LUNAR_FUNCTION( text_widget, get_text_id ),
        LUNAR_FUNCTION( text_widget, get_text ),
        LUNAR_FUNCTION_END
    };

    text_widget::text_widget( const widgetparams& params,
                              const std::shared_ptr<luaguicommandexecutor> lce,
                              const ilua_widget::functionnames& fnames,
                              const utf8text& textid,
                              const int textsize,
                              const std::shared_ptr<iwidgetrenderer> renderingStyle,
                              const std::shared_ptr<ilanguagetext> language )
        : ilua_widget( params,
                       iwidget::NoKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , text_id_( textid )
        , style_( renderingStyle )
        , language_( language )
        , textsize_( textsize )
    {
        assert( renderingStyle );
        assert( language );

        lunar<text_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    text_widget::text_widget( const widgetparams& params,
                              std::unique_ptr<ishape> shape,
                              const std::shared_ptr<luaguicommandexecutor> lce,
                              const ilua_widget::functionnames& fnames,
                              const utf8text& textid,
                              const int textsize,
                              const std::shared_ptr<iwidgetrenderer> renderingStyle,
                              const std::shared_ptr<ilanguagetext> language )
        : ilua_widget( params,
                       iwidget::NoKeyBoardFocus,
                       std::move( shape ),
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , text_id_( textid )
        , style_( renderingStyle )
        , language_( language )
        , textsize_( textsize )
    {
        assert( renderingStyle );
        assert( language );

        lunar<text_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    //!This method is never called.
    text_widget::text_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       std::shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , text_id_()
        , style_()
        , language_()
        , textsize_( 0 )
    {
        ss_throw( "Calling creation of text_widget via Lua is not allowed." );
    }

    void text_widget::render( const unsigned long ) const
    {
        const auto q = style_->screencoord_quad( active_area() );

        const position2df pos( 0.5f * q.width_ + q.x_, 0.5f * q.height_ + q.y_ );

        const auto state = this->is_widget_enabled() ? LooksNormal : LooksDisabled;

        style_->render_button_back( q, state );

        style_->render_text( get_button_text().c_str(),
                             textsize_,
                             pos,
                             dimension2df(),
                             HorizontalAlignCentre,
                             VerticalAlignCentre,
                             state );

        style_->render_button_highlight( q, state );
    }

    int text_widget::set_screen_quad( lua_State* L )
    {
        solosnake::rect r(
            static_cast<int>( luaL_checknumber( L, 1 ) ),
            static_cast<int>( luaL_checknumber( L, 2 ) ),
            dimension2dui( static_cast<int>( luaL_checknumber( L, 3 ) ),
                           static_cast<int>( luaL_checknumber( L, 4 ) ) ) );

        this->set_activearea_rect( r );

        return 0;
    }

    int text_widget::get_screen_quad( lua_State* L )
    {
        solosnake::rect r = this->active_area();
        lua_pushnumber( L, r.left() );
        lua_pushnumber( L, r.top() );
        lua_pushnumber( L, r.width() );
        lua_pushnumber( L, r.height() );
        return 4;
    }

    int text_widget::set_text_id( lua_State* L )
    {
        text_id_ = utf8text( luaL_checkstring( L, 1 ) );
        return 0;
    }

    utf8text text_widget::get_button_text() const
    {
        return language_->text_for( text_id_ );
    }

    int text_widget::get_text( lua_State* L )
    {
        utf8text translated = get_button_text();
        lua_pushlstring( L, translated.c_str(), translated.bytelength() );
        return 1;
    }

    int text_widget::get_text_id( lua_State* L )
    {
        lua_pushlstring( L, text_id_.c_str(), text_id_.bytelength() );
        return 1;
    }

    void text_widget::on_cursor_enter( screenxy xy, cursorbuttons_state s )
    {
        ilua_widget::on_cursor_enter( xy, s );
    }
}
