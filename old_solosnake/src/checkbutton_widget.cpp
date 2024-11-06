#include <cassert>
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/languagetexts.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/checkbutton_widget.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/lua/lunar.hpp"

using namespace std;

namespace solosnake
{
    LUNAR_CLASS_FUNCTIONS( checkbutton_widget ) =
    {
        LUNAR_FUNCTION( checkbutton_widget, set_screen_quad ),
        LUNAR_FUNCTION( checkbutton_widget, get_screen_quad ),
        LUNAR_FUNCTION( checkbutton_widget, set_text_id ),
        LUNAR_FUNCTION( checkbutton_widget, get_text_id ),
        LUNAR_FUNCTION( checkbutton_widget, get_text ),
        LUNAR_FUNCTION( checkbutton_widget, is_button_checked ),
        LUNAR_FUNCTION( checkbutton_widget, set_button_checked ),
        LUNAR_FUNCTION( checkbutton_widget, toggle_checked ),        
        LUNAR_FUNCTION( checkbutton_widget, set_button_enabled ),
        LUNAR_FUNCTION_END
    };

    checkbutton_widget::checkbutton_widget(
        const widgetparams& params,
        const shared_ptr<luaguicommandexecutor> lce,
        const ilua_widget::functionnames& fnames,
        const std::string& oncheckchanged_fname,
        const bool checked,
        const utf8text& textid,
        const int textsize,
        const shared_ptr<iwidgetrenderer> renderingStyle,
        const shared_ptr<ilanguagetext> language )
        : ilua_widget( params,
                       iwidget::NoKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , style_( renderingStyle )
        , language_( language )
        , on_check_changed_function_name_( oncheckchanged_fname )
        , text_id_( textid )
        , looks_()
        , textsize_( textsize )
        , checked_( checked )
    {
        assert( renderingStyle );
        assert( language );

        lunar<checkbutton_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    checkbutton_widget::checkbutton_widget(
        const widgetparams& params,
        unique_ptr<ishape> shape,
        const shared_ptr<luaguicommandexecutor> lce,
        const ilua_widget::functionnames& fnames,
        const std::string& oncheckchanged_fname,
        const bool checked,
        const utf8text& textid,
        const int textsize,
        const shared_ptr<iwidgetrenderer> renderingStyle,
        const shared_ptr<ilanguagetext> language )
        : ilua_widget( params,
                       iwidget::NoKeyBoardFocus,
                       move( shape ),
                       lce,
                       fnames,
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , style_( renderingStyle )
        , language_( language )
        , on_check_changed_function_name_( oncheckchanged_fname )
        , text_id_( textid )
        , looks_()
        , textsize_( textsize )
        , checked_( checked )
    {
        assert( renderingStyle );
        assert( language );

        lunar<checkbutton_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );

        call_init();
    }

    //!This method is never called.
    checkbutton_widget::checkbutton_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , style_()
        , language_()
        , on_check_changed_function_name_()
        , text_id_()
        , looks_()
        , textsize_( 0 )
        , checked_()
    {
        ss_throw( "Calling creation of checkbutton_widget via Lua is not allowed." );
    }

    WidgetVisualStates checkbutton_widget::get_visual_state( WidgetState actionButton ) const
    {
        return looks_.get_visual_state_for( actionButton );
    }

    void checkbutton_widget::render( const unsigned long ) const
    {
        const auto state = get_visual_state( activation_state() );

        const auto totalquad = style_->screencoord_quad( active_area() );

        // Left aligned text.
        const position2df textpos( totalquad.x_, 0.5f * totalquad.height_ + totalquad.y_ );

        // Checkbox is a square on the right hand side of the text rectangle.
        const quad checkquad( totalquad.width_ + totalquad.x_ - totalquad.height_,
                              totalquad.y_,
                              totalquad.height_,
                              totalquad.height_ );

        style_->render_button_back( totalquad, state );

        style_->render_text( get_button_text().c_str(),
                             textsize_,
                             textpos,
                             dimension2df(),
                             HorizontalAlignLeft,
                             VerticalAlignCentre,
                             state );

        style_->render_checkbox( checkquad, state, checked_ );

        style_->render_button_highlight( totalquad, state );
    }

    int checkbutton_widget::set_screen_quad( lua_State* L )
    {
        solosnake::rect r(
            static_cast<int>( luaL_checknumber( L, 1 ) ),
            static_cast<int>( luaL_checknumber( L, 2 ) ),
            dimension2dui( static_cast<int>( luaL_checknumber( L, 3 ) ),
                           static_cast<int>( luaL_checknumber( L, 4 ) ) ) );

        this->set_activearea_rect( r );

        return 0;
    }

    int checkbutton_widget::get_screen_quad( lua_State* L )
    {
        solosnake::rect r = this->active_area();
        lua_pushnumber( L, r.left() );
        lua_pushnumber( L, r.top() );
        lua_pushnumber( L, r.width() );
        lua_pushnumber( L, r.height() );
        return 4;
    }

    int checkbutton_widget::set_text_id( lua_State* L )
    {
        text_id_ = utf8text( luaL_checkstring( L, 1 ) );
        return 0;
    }

    utf8text checkbutton_widget::get_button_text() const
    {
        return language_->text_for( text_id_ );
    }

    int checkbutton_widget::get_text( lua_State* L )
    {
        utf8text translated = get_button_text();
        lua_pushlstring( L, translated.c_str(), translated.bytelength() );
        return 1;
    }

    int checkbutton_widget::get_text_id( lua_State* L )
    {
        lua_pushlstring( L, text_id_.c_str(), text_id_.bytelength() );
        return 1;
    }

    bool checkbutton_widget::is_button_checked() const
    {
        return checked_;
    }

    int checkbutton_widget::is_button_checked( lua_State* L )
    {
        lua_pushboolean( L, is_button_checked() ? 1 : 0 );
        return 1;
    }

    int checkbutton_widget::set_button_enabled( lua_State* L )
    {
        set_button_enabled( lua_toboolean(L,1) != 0 );
        return 0;
    }

    int checkbutton_widget::set_button_checked( lua_State* L )
    {
        const bool check = lua_toboolean( L, -1 ) > 0;
        set_button_checked( check );
        return 0;
    }

    void checkbutton_widget::set_button_checked( const bool checked )
    {
        if( checked_ != checked )
        {
            checked_ = checked;
            on_check_changed();
        }
    }

    void checkbutton_widget::on_check_changed()
    {
        if( false == on_check_changed_function_name_.empty() )
        {
            auto L = lua();

            // function to be called.
            lua_getglobal( L, on_check_changed_function_name_.c_str() );

            // Push 1st arg (self)
            lua_getglobal( L, name().str().c_str() );
            assert( lua_isuserdata( L, -1 ) );

            // Push 2nd arg (checked boolean)
            lua_pushboolean( L, checked_ ? 1 : 0 );

            // do the call (2 arguments, 0 results)
            if( lua_pcall( L, 2, 0, 0 ) != 0 )
            {
                ss_err( "Lua error calling '", on_check_changed_function_name_, 
                        "', (", lua_tostring( L, -1 ), ")" );
                lua_pop( L, 1 );
            }
        }
    }

    void checkbutton_widget::toggle_checked()
    {
        set_button_checked( ! is_button_checked() );
    }

    int checkbutton_widget::toggle_checked( lua_State* )
    {
        toggle_checked();
        return 0;
    }

    bool checkbutton_widget::is_button_enabled() const
    {
        return this->is_widget_enabled();
    }

    void checkbutton_widget::set_button_enabled( bool on )
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
}
