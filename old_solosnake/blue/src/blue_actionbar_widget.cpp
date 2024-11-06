#include "solosnake/blue/blue_actionbar_widget.hpp"
#include "solosnake/alignment.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/position.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/utf8text.hpp"
#include <cassert>

using namespace std;
using namespace solosnake;

namespace blue
{
    LUNAR_CLASS_FUNCTIONS( actionbar_widget ) =
    {
        LUNAR_FUNCTION_END
    };

    //! This method is never called.
    actionbar_widget::actionbar_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DisableIfNoClickedHandler,
                       iwidget::SharedPtrOnly() )
        , style_()
        , actionbar_()
        , button_states_()
        , background_texcoords_()
        , background_tex_()
        , button_sidelength_()
        , border_width_()
    {
        ss_throw( "Calling creation of actionbar_widget via Lua is not allowed." );
    }

    actionbar_widget::actionbar_widget(
        const std::shared_ptr<deck>& dck,
        const actionbar_params& params,
        const solosnake::widgetparams wparams,
        const solosnake::ilua_widget::functionnames& fnames,
        const std::shared_ptr<solosnake::luaguicommandexecutor>& lce,
        const std::shared_ptr<solosnake::iwidgetrenderer>& style )
        : ilua_widget( wparams,
                       iwidget::YesKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , style_( style )
        , actionbar_( make_unique<actionbar>(dck) )
        , button_states_()
        , background_texcoords_( params.background_tex_coords_ )
        , background_tex_( params.background_tex_handle_ )
        , button_sidelength_( params.button_sidelength_ )
        , border_width_( params.border_width_ )
    {
        button_states_.resize( actionbar_->button_count(), NormalButton );
        button_quads_.resize( actionbar_->button_count() );

        button_colours_[ NormalButton ]      = params.normal_colour_;
        button_colours_[ DisabledButton ]    = params.disabled_colour_;
        button_colours_[ HighlightedButton ] = params.highlighted_colour_;

        assert( style_ );
        lunar<actionbar_widget>::add_as_global( lce->lua(), this, wparams.name.c_str(), false );
        call_init();
    }

    rect actionbar_widget::get_button_screen_pixel_rect( const unsigned int i ) const
    {
        const auto area = active_area();
        const auto bx = area.left() + calculated_border_width_ + ( i * ( calculated_border_width_ + calculated_button_sidelength_ ) );
        const auto by = area.top()  + calculated_border_width_;
        return rect( bx, by, calculated_button_sidelength_, calculated_button_sidelength_ );
    }

    void actionbar_widget::render( const unsigned long ) const
    {
        // Draw background.
        deferred_renderer::screen_quad q;
        q.screenCoord = style_->screencoord_quad( active_area() );
        q.texCoord    = background_texcoords_;

        // Draw buttons.
        const auto nbuttons = actionbar_->button_count();
        button_quads_.resize( nbuttons );

        for( unsigned int i = 0u; i < nbuttons; ++i )
        {
            // TODO DAIRE FIX THIS ?
            // ss_wrn( "FIX THIS " __FILE__ )
            button_quads_[i].screenCoord = style_->screencoord_quad( get_button_screen_pixel_rect( i ) );
            button_quads_[i].texCoord    = background_texcoords_; // DAIRE TEMP HACK
            button_quads_[i].texCoord.x1 = 0.001f;//    = background_texcoords_; // DAIRE TEMP HACK
            button_quads_[i].texCoord.y1 = 0.001f;//    = background_texcoords_; // DAIRE TEMP HACK
            button_quads_[i].cornerColours[0] = button_colours_[ button_states_[i] ];
            button_quads_[i].cornerColours[1] = button_colours_[ button_states_[i] ];
            button_quads_[i].cornerColours[2] = button_colours_[ button_states_[i] ];
            button_quads_[i].cornerColours[3] = button_colours_[ button_states_[i] ];
        }

        style_->renderer().draw_screen_quads( background_tex_, &q, 1u );
        style_->renderer().draw_screen_quads( background_tex_, &button_quads_.at( 0 ), nbuttons );
    }

    void actionbar_widget::on_cursor_moved( screenxy xy, cursorbuttons_state )
    {
        const auto nbuttons = button_states_.size();

        for( unsigned int i = 0u; i < nbuttons; ++i )
        {
            if( get_button_screen_pixel_rect(i).contains( xy.screen_x(), xy.screen_y() ) )
            {
                button_states_[i] = HighlightedButton;
            }
            else
            {
                button_states_[i] = NormalButton;
            }
        }
    }
    
    void actionbar_widget::on_cursor_exit( screenxy, cursorbuttons_state )
    {
        const auto nbuttons = button_states_.size();

        for( unsigned int i = 0u; i < nbuttons; ++i )
        {
            button_states_[i] = NormalButton;
        }
    }

    void actionbar_widget::on_screen_resized( const dimension2d<unsigned int>& windowSize )
    {
        const auto nbuttons = actionbar_->button_count();

        // This is the desired back width and height. It may not be possible if the window is
        // too small in one or both dimensions.
        const auto backwidth  = ( button_sidelength_ * nbuttons ) + ( ( nbuttons + 1u ) * border_width_ );
        const auto backheight = ( 2u * border_width_ ) + button_sidelength_;

        const auto ww = windowSize.width();
        const auto wh = windowSize.height();

        // Clamp the action bar to a maximum width and height. Cannot be wider than
        // the screen, cannot be higher than a proportion of the height.
        const auto MaxProportionH = 8u;
        auto bw = backwidth  > ww ? ww : backwidth;
        auto bh = backheight > ( wh / MaxProportionH ) ? ( wh / MaxProportionH ) : backheight;

        // Check to see if we had to clamp the actionbar size to the window and
        // recalculate clamped size and border values.
        if( bw < backwidth || bh < backheight )
        {
            // See which dimension is furthest from the goal and use that.
            const float shrinkagew = static_cast<float>( bw ) / backwidth;
            const float shrinkageh = static_cast<float>( bh ) / backheight;

            if( shrinkagew < shrinkageh )
            {
                bh = static_cast<unsigned int>( backheight * shrinkagew );
                calculated_button_sidelength_ = static_cast<unsigned int>( button_sidelength_ * shrinkagew );
                calculated_border_width_      = static_cast<unsigned int>( border_width_ * shrinkagew );
            }
            else
            {
                bw = static_cast<unsigned int>( backwidth * shrinkageh );
                calculated_button_sidelength_ = static_cast<unsigned int>( button_sidelength_ * shrinkageh );
                calculated_border_width_      = static_cast<unsigned int>( border_width_ * shrinkageh );
            }
        }
        else
        {
            calculated_button_sidelength_ = button_sidelength_;
            calculated_border_width_      = border_width_;
        }

        const auto x = static_cast<int>( ( ww - bw ) / 2u );
        const auto y = static_cast<int>( wh - bh );
        const auto w = static_cast<int>( bw );
        const auto h = static_cast<int>( bh );

        set_activearea_rect( rect( x, y, w, h ) );
    }
}
