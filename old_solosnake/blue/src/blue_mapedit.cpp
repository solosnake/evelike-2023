#include "solosnake/blue/blue_create_gamecamera.hpp"
#include "solosnake/blue/blue_mapedit.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/screenxy.hpp"

namespace blue
{
    mapedit::mapedit( const user_settings& settings,
                      const std::shared_ptr<solosnake::rendering_system>& rs )
        : target_( mapedittarget() )
        , camera_( target_.target().grid_,
                   settings,
                   create_tiltcamera( target_.target().grid_, settings ),
                   rs )
    {
    }

    int mapedit::grid_height() const
    {
        return target().grid_.grid_height();
    }

    int mapedit::grid_width() const
    {
        return target().grid_.grid_width();
    }

    const hexgrid& mapedit::grid() const
    {
        return target().grid_;
    }

    pickresult mapedit::get_boardpick( solosnake::screenxy xy ) const
    {
        return camera_.get_boardpick( xy, target().grid_ );
    }

    solosnake::point3d mapedit::get_boardplanepick( solosnake::screenxy xy ) const
    {
        return camera_.get_boardplanepick( xy );
    }

    solosnake::point3d mapedit::camera_location() const
    {
        return camera_.location();
    }

    solosnake::point3d mapedit::camera_lookedat() const
    {
        return camera_.lookedat();
    }

    void mapedit::translate_camera( float x, float z )
    {
        return camera_.translate( x, z );
    }

    void mapedit::rotate_camera( solosnake::radians a )
    {
        return camera_.rotate( a );
    }

    void mapedit::move_camera_forwards( float units )
    {
        camera_.move_towards_lookat( units );
    }

    void mapedit::setup_scene_view_and_proj_matrices()
    {
        camera_.setup_scene_view_and_proj_matrices();
    }

    //void mapedit::set_tile_as( const Hex_coord tile, const MapEditBrushType )
    //{
    //    grid_.set_contents( tile, offboard_tile_value() );
    //}

}
