#include <cassert>
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/picturebutton_widget.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        texquad load_texquad( const solosnake::rectf& r )
        {
            return texquad( r.left(), r.top(), r.left() + r.width(), r.top() + r.height() );
        }
    }

    picturebutton_widget::picturebutton_widget( const widgetparams& params,
                                                const iwidget::KeyboardFocusing kbf,
                                                const solosnake::rectf& texturearea,
                                                const string& textureName,
                                                const shared_ptr<luaguicommandexecutor>& lce,
                                                const shared_ptr<iwidgetrenderer>& renderingStyle,
                                                const ilua_widget::functionnames& fnames,
                                                const utf8text& txt,
                                                const int textsize,
                                                const shared_ptr<ilanguagetext>& language )
        : ilua_widget( params, kbf, lce, fnames,
                       ilua_widget::DisableIfNoClickedHandler, iwidget::SharedPtrOnly() )
        , style_( renderingStyle )
        , language_( language )
        , looks_()
        , texcoords_( load_texquad( texturearea ) )
        , text_id_( txt )
        , textsize_( textsize )
        , page_( texturehandle_t() )
    {
        assert( language );
        assert( renderingStyle );

        load_and_set_texture( textureName );

        lunar<picturebutton_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    LUNAR_CLASS_FUNCTIONS( picturebutton_widget ) =
    {
        LUNAR_FUNCTION( picturebutton_widget, load_and_set_texture ),
        LUNAR_FUNCTION( picturebutton_widget, set_screen_quad ),
        LUNAR_FUNCTION( picturebutton_widget, set_texture_quad ),
        LUNAR_FUNCTION( picturebutton_widget, get_screen_quad ),
        LUNAR_FUNCTION( picturebutton_widget, get_texture_quad ),
        LUNAR_FUNCTION( picturebutton_widget, set_button_enabled ),
        LUNAR_FUNCTION( picturebutton_widget, set_text_id ),
        LUNAR_FUNCTION( picturebutton_widget, get_text_id ),
        LUNAR_FUNCTION( picturebutton_widget, set_text_size ),
        LUNAR_FUNCTION( picturebutton_widget, get_text_size ),
        LUNAR_FUNCTION( picturebutton_widget, get_text ),
        LUNAR_FUNCTION_END
    };

    // This method is never called.
    picturebutton_widget::picturebutton_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , style_()
        , language_()
        , looks_()
        , texcoords_()
        , text_id_()
        , textsize_()
        , page_()
    {
        ss_throw( "Calling creation of picturebutton_widget via Lua is not allowed." );
    }

    WidgetVisualStates picturebutton_widget::get_visual_state( WidgetState actionButton ) const
    {
        return looks_.get_visual_state_for(actionButton);
    }

    void picturebutton_widget::render( const unsigned long ) const
    {
        deferred_renderer::screen_quad q;

        q.screenCoord = style_->screencoord_quad( active_area() );
        q.texCoord    = texcoords_;

        style_->renderer().draw_screen_quads( page_, &q, 1u );
        
        if( ! text_id_.is_empty() )
        {
            const position2df textpos( 0.5f * q.screenCoord.width_ + q.screenCoord.x_, 
                                      0.5f * q.screenCoord.height_ + q.screenCoord.y_ );
        
            const auto state = get_visual_state( activation_state() );

            style_->render_text( get_button_text().c_str(),
                                 textsize_,
                                 textpos,
                                 dimension2df(),
                                 HorizontalAlignCentre,
                                 VerticalAlignCentre,
                                 state );
        }
    }

    void picturebutton_widget::load_and_set_texture( const string& textureName )
    {
        page_ = style_->renderingsystem().cache().get_texture( textureName );
    }

    int picturebutton_widget::load_and_set_texture( lua_State* L )
    {
        const char* tex = luaL_checkstring( L, 1 );

        try
        {
            load_and_set_texture( tex );
        }
        catch( const SS_EXCEPTION_TYPE& e )
        {
            return luaL_error( L, "Error loading screen quads texture '%s'.", e.what() );
        }

        return 0;
    }

    int picturebutton_widget::set_screen_quad( lua_State* L )
    {
        solosnake::rect r(
            static_cast<int>( luaL_checknumber( L, 1 ) ),
            static_cast<int>( luaL_checknumber( L, 2 ) ),
            dimension2dui( static_cast<unsigned int>( luaL_checknumber( L, 3 ) ),
                           static_cast<unsigned int>( luaL_checknumber( L, 4 ) ) ) );

        this->set_activearea_rect( r );

        return 0;
    }

    int picturebutton_widget::set_texture_quad( lua_State* L )
    {
        texcoords_.x0 = static_cast<float>( luaL_checknumber( L, 1 ) );
        texcoords_.y0 = static_cast<float>( luaL_checknumber( L, 2 ) );
        texcoords_.x1 = texcoords_.x0 + static_cast<float>( luaL_checknumber( L, 3 ) );
        texcoords_.y1 = texcoords_.y0 + static_cast<float>( luaL_checknumber( L, 4 ) );

        return 0;
    }

    int picturebutton_widget::get_screen_quad( lua_State* L )
    {
        solosnake::rect r = this->active_area();
        lua_pushnumber( L, r.left() );
        lua_pushnumber( L, r.top() );
        lua_pushnumber( L, r.width() );
        lua_pushnumber( L, r.height() );

        return 4;
    }

    int picturebutton_widget::get_texture_quad( lua_State* L )
    {
        lua_pushnumber( L, texcoords_.x0 );
        lua_pushnumber( L, texcoords_.y0 );
        lua_pushnumber( L, texcoords_.x1 - texcoords_.x0 );
        lua_pushnumber( L, texcoords_.y1 - texcoords_.y0 );

        return 4;
    }

    int picturebutton_widget::set_button_enabled( lua_State* L )
    {
        set_button_enabled( lua_toboolean( L, 1 ) != 0 );
        return 0;
    }

    bool picturebutton_widget::is_button_enabled() const
    {
        return this->is_widget_enabled();
    }

    void picturebutton_widget::set_button_enabled( bool on )
    {
        if( on )
        {
            this->enable_widget();
        }
        else
        {
            this->disable_widget();
        }
    }

    utf8text picturebutton_widget::get_button_text() const
    {
        return language_->text_for( text_id_ );
    }

    int picturebutton_widget::get_text( lua_State* L )
    {
        utf8text translated = get_button_text();
        lua_pushlstring( L, translated.c_str(), translated.bytelength() );
        return 1;
    }

    int picturebutton_widget::set_text_size( lua_State* L )
    {
        textsize_ = static_cast<int>( luaL_checknumber( L, 1 ) );
        return 0;
    }

    int picturebutton_widget::get_text_size( lua_State* L )
    {
        lua_pushnumber( L, textsize_ );
        return 1;
    }

    int picturebutton_widget::set_text_id( lua_State* L )
    {
        text_id_ = utf8text( luaL_checkstring( L, 1 ) );
        return 0;
    }

    int picturebutton_widget::get_text_id( lua_State* L )
    {
        lua_pushlstring( L, text_id_.c_str(), text_id_.bytelength() );
        return 1;
    }

}
