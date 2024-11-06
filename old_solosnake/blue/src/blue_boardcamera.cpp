#include <limits>
#include "solosnake/blue/blue_boardcamera.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/linesegment.hpp"
#include "solosnake/rendering_system.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        float get_max_view_distance( const iboardcamera& cam, const hexgrid& grid )
        {
            const float diagonal    = grid.get_shortside_3d_box_diagonal();
            const float maxViewDist = diagonal + cam.max_distance_to_lookat();
            return maxViewDist;
        }

        float get_z_near( const user_settings& settings )
        {
            return settings.value( BLUE_GAME_Z_NEAR ).as_float();
        }

        float get_z_far( const user_settings& settings,
                         const iboardcamera& cam,
                         const hexgrid& grid )
        {
            return get_z_near( settings ) + get_max_view_distance( cam, grid );
        }

        float get_view_angle( const user_settings& settings )
        {
            return degrees( settings.value( BLUE_GAME_VIEWANGLE ).as_float() ).to_radians();
        }
    }

    boardcamera::boardcamera( const hexgrid& grid,
                              const user_settings& settings,
                              unique_ptr<iboardcamera> cam,
                              const shared_ptr<rendering_system>& r )
        : camera_()
        , rendering_( r )
        , zNear_( get_z_near( settings ) )
        , zFar_( get_z_far( settings, *cam, grid ) )
        , viewAngle_( get_view_angle( settings ) )
    {
        camera_ = move( cam );
        update_view_and_proj();
    }

    boardcamera::~boardcamera()
    {
    }

    rendering_system* boardcamera::update_view_and_proj()
    {
        assert( rendering_ );
        auto windowrect = rendering_->get_window_rect();
        auto aspect     = static_cast<float>( windowrect.width() ) / windowrect.height();
        camera_->get_view_matrix( view_ );
        load_perspective( viewAngle_, aspect, zNear_, zFar_, proj_ );
        return rendering_.get();
    }

    void boardcamera::setup_scene_view_and_proj_matrices()
    {
        auto r = update_view_and_proj();
        r->renderer().set_proj_matrix( proj_, zNear_ );
        r->renderer().set_view_matrix( view_ );
    }

    float boardcamera::min_distance_to_lookat() const
    {
        return camera_->min_distance_to_lookat();
    }

    float boardcamera::max_distance_to_lookat() const
    {
        return camera_->max_distance_to_lookat();
    }

    point3d boardcamera::location() const
    {
        return camera_->location();
    }

    point3d boardcamera::upwards() const
    {
        return camera_->upwards();
    }

    point3d boardcamera::forwards() const
    {
        return camera_->forwards();
    }

    point3d boardcamera::lookedat() const
    {
        return camera_->lookedat();
    }

    void boardcamera::get_view_matrix( float* unaliased m4x4 ) const
    {
        camera_->get_view_matrix( m4x4 );
    }

    void boardcamera::get_orientation( float* unaliased up,
                                       float* unaliased side,
                                       float* unaliased ahead ) const
    {
        camera_->get_orientation( up, side, ahead );
    }

    void boardcamera::rotate( const radians r )
    {
        camera_->rotate( r );
    }

    void boardcamera::translate( const float x, const float z )
    {
        camera_->translate( x, z );
    }

    void boardcamera::move_towards_lookat( const float amount )
    {
        camera_->move_towards_lookat( amount );
    }

    pickresult boardcamera::get_boardpick( screenxy xy, const hexgrid& board ) const
    {
        pickresult result;

        rendering_->renderer().un_project( xy.screen_x(), xy.screen_y(), 1.0f, result.worldPoint.xyz );

        ss_dbg( "camera    ",
                camera_->location()[0],
                " ",
                camera_->location()[1],
                " ",
                camera_->location()[2] );

        const halfline3d line( camera_->location(), result.worldPoint );
        auto hex = board.get_line_intersection( line, BLUE_BOARD_Y );

        result.onboard = ( 0 == ( hex.tile_contents & board.invalid_tile_value() ) );
        result.location_contents = hex.tile_contents;
        result.boardcoord        = hex.tile_coordinate;

        return result;
    }

    point3d boardcamera::get_boardplanepick( screenxy xy ) const
    {
        point3d boardPoint;

        point3d pickEnd;
        rendering_->renderer().un_project( xy.screen_x(), xy.screen_y(), 1.0f, pickEnd.xyz );

        halfline3d eyeToBoardLine( camera_->location(), pickEnd );
        if( !find_halfline3d_yplane_intersection( eyeToBoardLine, BLUE_BOARD_Y, boardPoint.xyz ) )
        {
            //assert( !"NAN in board pick" );
            boardPoint.xyz[0] = numeric_limits<float>::quiet_NaN();
            boardPoint.xyz[1] = numeric_limits<float>::quiet_NaN();
            boardPoint.xyz[2] = numeric_limits<float>::quiet_NaN();
        }

        return boardPoint;
    }
}
