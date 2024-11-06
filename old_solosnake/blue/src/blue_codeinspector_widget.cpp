#include <cassert>
#include <iomanip>
#include <sstream>
#include "solosnake/blue/blue_codeinspector_widget.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_igameview.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_player.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/lua/lunar.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/utf8text.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    //! Initially creates a widget with size 1 x 1 but this is resized to
    //! correct screen size by the required "on_screen_resized" call of the
    //! widget system. The true and the -1 z depth ensures we get focus by
    //! default.
    codeinspector_widget::codeinspector_widget(
        const widgetparams& params,
        const shared_ptr<player>& player,
        const shared_ptr<compiler>& c,
        const std::weak_ptr<igameview>& pgameview,
        const std::shared_ptr<luaguicommandexecutor> lce,
        const ilua_widget::functionnames& fnames,
        const int textsize,
        const shared_ptr<iwidgetrenderer>& r )
        : ilua_widget( params,
                       iwidget::YesKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , renderer_( r )
        , player_( player )
        , compiler_( c )
        , code_as_text_()
        , gameview_( pgameview )
        , watched_machine_( nullptr )
        , visual_state_( LooksActivatedAndHasKeyboardFocus )
        , textsize_( textsize )
    {
        assert( ! gameview_.expired() );
        assert( player_ );
        assert( compiler_ );
        assert( renderer_ );
        lunar<codeinspector_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );
        call_init();
    }

    //! This method is never called.
    codeinspector_widget::codeinspector_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       std::shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , renderer_()
        , player_()
        , compiler_()
        , code_as_text_()
        , gameview_()
        , watched_machine_( nullptr )
        , visual_state_( LooksActivatedAndHasKeyboardFocus )
        , textsize_( 0 )
    {
        ss_throw( "Calling creation of codeinspector_widget via Lua is not allowed." );
    }

    codeinspector_widget::~codeinspector_widget()
    {
        if( watched_machine_ )
        {
            watched_machine_->dec_watched_refcount();
            watched_machine_ = nullptr;
        }
    }

    LUNAR_CLASS_FUNCTIONS( codeinspector_widget ) =
    {
        LUNAR_FUNCTION( codeinspector_widget, set_screen_quad ),
        LUNAR_FUNCTION_END
    };

    void codeinspector_widget::render( const unsigned long ) const
    {
        if( watched_machine_ )
        {
            solosnake::utf8text name( watched_machine_->class_name() );

            const auto startletterpos = solosnake::position2df( 0.0f, 0.0f );

            const auto lastletterpos = renderer_->render_text( name,
                                                               textsize_,
                                                               startletterpos,
                                                               dimension2df(),
                                                               HorizontalAlignLeft,
                                                               VerticalAlignTop,
                                                               visual_state_ );

            // Start below the
            auto nextletterpos = solosnake::position2df( startletterpos.x(),
                                                         lastletterpos.second.y() );

            renderer_->render_text( code_as_text_,
                                    textsize_,
                                    nextletterpos,
                                    dimension2df(),
                                    HorizontalAlignLeft,
                                    VerticalAlignTop,
                                    visual_state_ );
        }
    }

    void codeinspector_widget::on_double_clicked( unsigned short button, screenxy xy )
    {
        ss_log( "codeinspector_widget::on_double_clicked" );

        ilua_widget::on_double_clicked( button, xy );

        if( watched_machine_ )
        {
            watched_machine_->dec_watched_refcount();
            watched_machine_ = nullptr;
        }

        shared_ptr<igameview> gv( gameview_ );

        pickresult pick = gv->get_boardpick( xy );

        if( pick.onboard )
        {
            watched_machine_ = gv->get_machine_at( pick.boardcoord );

            if( watched_machine_ )
            {
                watched_machine_->inc_watched_refcount();
            }

            update_code();
        }
    }

    void codeinspector_widget::update_code()
    {
        if( watched_machine_ )
        {
            if( watched_machine_->is_alive() )
            {
                stringstream ss;
                const instructions& code = watched_machine_->code();
                for( size_t i = 1u; i <= code.size(); ++i )
                {
                    // Line number
                    ss << setfill( stringstream::char_type( '0' ) )
                       << setw( 5 )
                       << i;

                    // Code
                    ss << stringstream::char_type( ' ' )
                       << stringstream::char_type( ' ' )
                       << stringstream::char_type( ' ' )
                       << compiler_->decompile( code[i] )
                       << stringstream::char_type( '\n' );
                }

                code_as_text_ = ss.str();
            }
            else
            {
                watched_machine_->dec_watched_refcount();
                watched_machine_ = nullptr;
            }
        }
    }

    int codeinspector_widget::set_screen_quad( lua_State* L )
    {
        solosnake::rect r( static_cast<int>( luaL_checknumber( L, 1 ) ),
                           static_cast<int>( luaL_checknumber( L, 2 ) ),
                           dimension2dui( static_cast<unsigned int>( luaL_checknumber( L, 3 ) ),
                                          static_cast<unsigned int>( luaL_checknumber( L, 4 ) ) ) );

        this->set_activearea_rect( r );

        return 0;
    }

    void codeinspector_widget::advance_one_frame()
    {
        update_code();
    }
}
