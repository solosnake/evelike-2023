#ifndef blue_mapedit_hpp
#define blue_mapedit_hpp

#include <vector>
#include "solosnake/commandpattern.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_boardcamera.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_iboardview.hpp"
#include "solosnake/blue/blue_mapedittarget.hpp"
#include "solosnake/blue/blue_mapeditbrushtype.hpp"
#include "solosnake/blue/blue_sun.hpp"

namespace blue
{
    class user_settings;

    //! The mapedit state.
    class mapedit : public iboardview
    {
    public:

        typedef solosnake::command<mapedittarget> command_t;

        //!
        mapedit(
            const user_settings&,
            const std::shared_ptr<solosnake::rendering_system>& );

        //! Width refers to the X coordinate.
        int grid_width() const;

        //! Height refers to Y coordinate.
        int grid_height() const;

        const Hex_grid& grid() const;

        pickresult get_boardpick( solosnake::screenxy xy ) const override;

        //! Assumes the eye is looking towards the board, returns the
        //! pick point of the screen position xy in the board plane.
        solosnake::point3d get_boardplanepick( solosnake::screenxy xy ) const override;

        solosnake::point3d camera_location() const override;

        solosnake::point3d camera_lookedat() const override;

        void translate_camera( float x, float z ) override;

        void rotate_camera( solosnake::radians ) override;

        void move_camera_forwards( float units ) override;

        void setup_scene_view_and_proj_matrices();

        void do_command( std::unique_ptr<command_t> );

        void undo_command();

        void redo_command();

    private:

        const mapedittarget& target() const { return target_.target(); }

    private:

        solosnake::commandhistory<mapedittarget>    target_;
        boardcamera                                 camera_;
    };
}

#include "solosnake/blue/blue_mapedit.inl"

#endif
