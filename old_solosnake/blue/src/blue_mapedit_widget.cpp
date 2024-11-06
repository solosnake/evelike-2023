#include <cassert>
#include <utility>
#include "solosnake/rendering_system.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/input_event.hpp"
#include "solosnake/vkeys.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/blue/blue_drawboard.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_mapedit_widget.hpp"
#include "solosnake/blue/blue_mapeditcommands.hpp"
#include "solosnake/blue/blue_settings.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    //! Specialised mapedit board drawing class. Colours the hexagonal
    //! tiles in showing all the possible tiles (not just the on-board
    //! tiles).
    class mapedit_widget::boarddrawer : public drawboard
    {
    public:

        boarddrawer( rendering_system&, const persistance& settings );

        void set_highlighted( const Hex_coord& );

        void set_highlighted( const vector<Hex_coord>& );

    private:

        static drawboardparams load_settings( rendering_system&,  const persistance& );

        void set_board_colours( const hexgrid&, const size_t, dynbufferRGBA* ) const override;

    private:

        std::vector<Hex_coord> highlighted_tiles_;
    };

    mapedit_widget::boarddrawer::boarddrawer( rendering_system& r, const persistance& settings )
        : drawboard( r, load_settings( r, settings ) )
    {
    }

    drawboardparams mapedit_widget::boarddrawer::load_settings( rendering_system& r,
                                                                const persistance& settings )
    {
        drawboardparams params;

        params.transparency_       = settings.value( BLUE_GAME_MAP_TILEALPHA ).as_float();
        params.tile_shrink_factor_ = settings.value( BLUE_GAME_MAP_TILESHRINKFACTOR ).as_float();
        params.tx_ = settings.value( BLUE_GAME_MAP_TILETEXTURE_X ).as_float();
        params.ty_ = settings.value( BLUE_GAME_MAP_TILETEXTURE_Y ).as_float();
        params.tw_ = settings.value( BLUE_GAME_MAP_TILETEXTURE_XW ).as_float();
        params.th_ = settings.value( BLUE_GAME_MAP_TILETEXTURE_YH ).as_float();

        // Check as much as possible before allocating any resources:
        const auto& texname = settings.value( BLUE_GAME_MAP_TILETEXTURE ).str();

        if( texname.empty() )
        {
            ss_throw( "No board texture specified." );
        }

        if( params.transparency_ < 0.0f || params.transparency_ > 1.0f )
        {
            ss_throw( BLUE_GAME_TILEALPHA " is out of range." );
        }

        if( params.tile_shrink_factor_ <= 0.0f || params.tile_shrink_factor_ > 1.0f )
        {
            ss_throw( BLUE_GAME_TILESHRINKFACTOR " is out of range." );
        }

        params.texture_ = r.get_cache_ptr()->get_texture( texname );

        return params;
    }

    void mapedit_widget::boarddrawer::set_highlighted( const Hex_coord& tile )
    {
        highlighted_tiles_.resize( 1u );
        highlighted_tiles_[0] = tile;
    }

    void mapedit_widget::boarddrawer::set_highlighted( const vector<Hex_coord>& tiles )
    {
        highlighted_tiles_ = tiles;
    }

    void mapedit_widget::boarddrawer::set_board_colours( const hexgrid& g,
                                                         const size_t tileCount,
                                                         dynbufferRGBA* rgbas ) const
    {

        const dynbufferRGBA offboardColour = { 64u, 64u, 64u, 128u };

        const dynbufferRGBA tilecolours[3] =
        {
            { 255u, 128u, 128u, 128u },
            { 128u, 255u, 128u, 128u },
            { 128u, 128u, 255u, 128u },
        };

        const dynbufferRGBA colors[2][3] =
        {
            { tilecolours[0], tilecolours[1], tilecolours[2] },
            { tilecolours[2], tilecolours[0], tilecolours[1] }
        };

        const auto h = g.grid_height();
        const auto w = g.grid_width();
        assert( tileCount == static_cast<size_t>( w * h ) );

        auto dst = rgbas;

        for( int8_t y = 0; y < h; ++y )
        {
            for( int8_t x = 0; x < w; ++x )
            {
                const auto xy      = Hex_coord::make_coord( x, y );
                const auto isTile  = is_not_offboard_tile( g.contents( xy ) );

                const auto color = isTile ? ( colors[x % 2][y % 3] ) : offboardColour;

                dst[0] = color;
                dst[1] = color;
                dst[2] = color;
                dst[3] = color;
                dst[4] = color;
                dst[5] = color;

                dst += 6u;

                assert( ( static_cast<size_t>( dst - rgbas ) / 6u ) <= tileCount );
            }
        }

        const dynbufferRGBA highlightColour = { 255u, 64u, 64u, 255u };
        const auto n = highlighted_tiles_.size();
        for( size_t i = 0u; i < n; ++i )
        {
            if( highlighted_tiles_[i].is_valid() )
            {
                auto highlight = rgbas + ( 6 * ( highlighted_tiles_[i].x + ( w * highlighted_tiles_[i].y ) ) );

                assert( ( static_cast<size_t>( highlight - rgbas ) / 6u ) <= tileCount );

                highlight[0] = highlightColour;
                highlight[1] = highlightColour;
                highlight[2] = highlightColour;
                highlight[3] = highlightColour;
                highlight[4] = highlightColour;
                highlight[5] = highlightColour;
            }
        }
    }

    LUNAR_CLASS_FUNCTIONS( mapedit_widget ) =
    {
        LUNAR_FUNCTION( mapedit_widget, get_screen_quad ),
        LUNAR_FUNCTION( mapedit_widget, set_brush_type ),
        LUNAR_FUNCTION( mapedit_widget, make_brush_bigger ),
        LUNAR_FUNCTION( mapedit_widget, make_brush_smaller ),
        LUNAR_FUNCTION_END
    };

    // This method is never called.
    mapedit_widget::mapedit_widget( lua_State* )
        : gameviewing_widget()
        , renderer_()
        , drawboard_()
        , board_()
        , current_mode_( EditingMapMode )
        , key_undo_( 'Z' )
        , key_redo_( 'Y' )
        , key_change_mode_( VK_SHIFT )
    {
        ss_throw( "Calling creation of picturebutton_widget via Lua is not allowed." );
    }

    mapedit_widget::mapedit_widget( const widgetparams& params,
                                    const persistance& settings,
                                    const shared_ptr<mapedit>& sharedmap,
                                    const shared_ptr<luaguicommandexecutor>& lce,
                                    const shared_ptr<iwidgetrenderer>& r,
                                    const ilua_widget::functionnames& fnames )
        : gameviewing_widget( params,
                              iwidget::YesKeyBoardFocus,
                              ilua_widget::DefaultEnabled,
                              static_pointer_cast<iboardview>( sharedmap ),
                              lce,
                              r,
                              fnames )
        , renderer_( r )
        , drawboard_( make_unique<mapedit_widget::boarddrawer>( renderer_->renderingsystem(), settings ) )
        , board_( sharedmap )
        , current_mode_( EditingMapMode )
        , key_undo_( 'Z' )
        , key_redo_( 'Y' )
        , key_change_mode_( VK_SHIFT )
    {
        assert( r );

        modes_[ EditingMapMode ].clicked_on_board  = & mapedit_widget::editmode_clicked_on_board;
        modes_[ EditingMapMode ].key_pressed       = & mapedit_widget::editmode_key_pressed;
        modes_[ EditingMapMode ].key_released      = & mapedit_widget::editmode_key_released;
        modes_[ EditingMapMode ].cursor_moved      = & mapedit_widget::editmode_cursor_moved;
        modes_[ EditingMapMode ].cursor_enter      = & mapedit_widget::editmode_cursor_enter;
        modes_[ EditingMapMode ].wheelmoved_inside = & mapedit_widget::editmode_wheelmoved_inside;
        modes_[ EditingMapMode ].reset             = & mapedit_widget::editmode_reset;

        modes_[ MovementMode ].clicked_on_board    = & mapedit_widget::movemode_clicked_on_board;
        modes_[ MovementMode ].key_pressed         = & mapedit_widget::movemode_key_pressed;
        modes_[ MovementMode ].key_released        = & mapedit_widget::movemode_key_released;
        modes_[ MovementMode ].cursor_moved        = & mapedit_widget::movemode_cursor_moved;
        modes_[ MovementMode ].cursor_enter        = & mapedit_widget::movemode_cursor_enter;
        modes_[ MovementMode ].wheelmoved_inside   = & mapedit_widget::movemode_wheelmoved_inside;
        modes_[ MovementMode ].reset               = & mapedit_widget::movemode_reset;

        lunar<mapedit_widget>::add_as_global( lce->lua(), this, params.name.c_str(), false );
        call_init();
    }

    mapedit_widget::~mapedit_widget()
    {
        drawboard_->release_board_buffer();
    }

    void mapedit_widget::render( const unsigned long ) const
    {
        drawboard_->make_board_buffer( board_->grid(), false );
        drawboard_->update_board_colours( board_->grid() );
        drawboard_->render();
    }

    int mapedit_widget::get_screen_quad( lua_State* L )
    {
        auto r = this->active_area();
        lua_pushnumber( L, r.left() );
        lua_pushnumber( L, r.top() );
        lua_pushnumber( L, r.width() );
        lua_pushnumber( L, r.height() );

        return 4;
    }

    mapedit_widget::mode_handler& mapedit_widget::handler()
    {
        return modes_[ current_mode_ ];
    }

    int mapedit_widget::make_brush_bigger( lua_State* )
    {
        return 0;
    }

    int mapedit_widget::make_brush_smaller( lua_State* )
    {
        return 0;
    }

    int mapedit_widget::set_brush_type( lua_State* L )
    {
        const char* brush = luaL_checkstring( L, 1 );
        ss_log( brush );
        return 0;
    }

    void mapedit_widget::set_mode( const MapIntaractionMode m )
    {
        current_mode_ = m;
    }

    void mapedit_widget::change_map( unique_ptr<command<mapedittarget>> cmd )
    {
        board_->do_command( move( cmd ) );
    }

    void mapedit_widget::set_highlighted( const Hex_coord& tile )
    {
        drawboard_->set_highlighted( tile );
    }

    void mapedit_widget::set_highlighted( const vector<Hex_coord>& tiles )
    {
        drawboard_->set_highlighted( tiles );
    }

    void mapedit_widget::on_key_pressed( unsigned short k, unsigned int modifierFlags )
    {
        ( this->*( handler().key_pressed ) )( k, modifierFlags );
    }

    void mapedit_widget::on_key_released( unsigned short k, unsigned int modifierFlags )
    {
        ( this->*( handler().key_released ) )( k, modifierFlags );
    }

    void mapedit_widget::on_clicked_on_board( const Hex_coord& xy )
    {
        ( this->*( handler().clicked_on_board ) )( xy );
    }

    void mapedit_widget::on_cursor_moved( screenxy xy, cursorbuttons_state s )
    {
        ( this->*( handler().cursor_moved ) )( xy, s );
    }

    void mapedit_widget::on_cursor_enter( screenxy xy, cursorbuttons_state s )
    {
        ( this->*( handler().cursor_enter ) )( xy, s );
    }

    void mapedit_widget::on_wheelmoved_inside( screenxy xy, float s )
    {
        ( this->*( handler().wheelmoved_inside ) )( xy, s );
    }

    void mapedit_widget::on_reset()
    {
        ( this->*( handler().reset ) )();
    }

    // Move Mode //////////////////////////////////////////////////////////////////////////////////

    void mapedit_widget::movemode_clicked_on_board( const Hex_coord& xy )
    {
        gameviewing_widget::on_clicked_on_board( xy );
    }

    void mapedit_widget::movemode_key_pressed( unsigned short k, unsigned int modifierFlags )
    {
        gameviewing_widget::on_key_pressed( k, modifierFlags );
    }

    void mapedit_widget::movemode_key_released( unsigned short k, unsigned int modifierFlags )
    {
        if( k == key_change_mode_ )
        {
            set_mode( EditingMapMode );
            drawboard_->set_highlighted( Hex_coord::make_invalid_coord() );
        }
        else
        {
            gameviewing_widget::on_key_released( k, modifierFlags );
        }
    }

    void mapedit_widget::movemode_cursor_moved( screenxy xy, cursorbuttons_state s )
    {
        gameviewing_widget::on_cursor_moved( xy, s );
    }

    void mapedit_widget::movemode_cursor_enter( screenxy xy, cursorbuttons_state s )
    {
        gameviewing_widget::on_cursor_enter( xy, s );
    }

    void mapedit_widget::movemode_wheelmoved_inside( screenxy xy, float s )
    {
        gameviewing_widget::on_wheelmoved_inside( xy, s );
    }

    void mapedit_widget::movemode_reset()
    {
        drawboard_->set_highlighted( Hex_coord::make_invalid_coord() );
    }

    // Edit Mode //////////////////////////////////////////////////////////////////////////////////

    void mapedit_widget::editmode_clicked_on_board( const Hex_coord& )
    {
    }

    void mapedit_widget::editmode_key_pressed( unsigned short k, unsigned int modifierFlags )
    {
        if( k == key_undo_ && input_event::is_ctrl_down( modifierFlags ) )
        {
            board_->undo_command();
        }
        else if( k == key_redo_ && input_event::is_ctrl_down( modifierFlags ) )
        {
            board_->redo_command();
        }
        else if( k == key_change_mode_ )
        {
            set_mode( MovementMode );
        }
        else
        {
            gameviewing_widget::on_key_pressed( k, modifierFlags );
        }
    }

    void mapedit_widget::editmode_key_released( unsigned short k, unsigned int modifierFlags )
    {
        gameviewing_widget::on_key_released( k, modifierFlags );
    }

    void mapedit_widget::editmode_cursor_moved( screenxy xy, cursorbuttons_state )
    {
        const auto pick = board_->get_boardpick( xy );

        if( pick.boardcoord.is_valid() && pick.location_contents != offboard_tile_value() )
        {
            board_->do_command( make_unique<changetile>( pick.boardcoord, offboard_tile_value() ) );
        }

        drawboard_->set_highlighted( board_->get_boardpick( xy ).boardcoord );
    }

    void mapedit_widget::editmode_cursor_enter( screenxy, cursorbuttons_state )
    {
    }

    void mapedit_widget::editmode_wheelmoved_inside( screenxy xy, float )
    {
        drawboard_->set_highlighted( board_->get_boardpick( xy ).boardcoord );
    }

    void mapedit_widget::editmode_reset()
    {
    }
}
