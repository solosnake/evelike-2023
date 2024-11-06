#ifndef blue_boarcamera_hpp
#define blue_boarcamera_hpp

#include <memory>
#include "solosnake/blue/blue_iboardcamera.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/screenxy.hpp"
#include "solosnake/matrix3d.hpp"

namespace blue
{
    class Hex_grid;
    class user_settings;

    //! A screen camera is a helper camera that contains and manages a view and project matrix,
    //! and which takes care of many of the tasks associated with the camera and its interaction
    //! with the renderer. It still requires notification of window resizes etc.
    class boardcamera : public iboardcamera
    {
    public:

        boardcamera(
            const Hex_grid&,
            const user_settings&,
            std::unique_ptr<iboardcamera>,
            const std::shared_ptr<solosnake::rendering_system>& );

        ~boardcamera();

        const float* get_view_matrix() const;

        const float* get_vproj_matrix() const;

        float min_distance_to_lookat() const override;

        float max_distance_to_lookat() const override;

        solosnake::point3d location() const override;

        solosnake::point3d upwards() const override;

        solosnake::point3d forwards() const override;

        solosnake::point3d lookedat() const override;

        //! Sets into the 4x4 matrix the view matrix representing the current
        //! camera's position and orientation.
        void get_view_matrix( float* unaliased m4x4 ) const override;

        void get_orientation(
            float* unaliased up,
            float* unaliased side,
            float* unaliased ahead ) const override;

        //! Rotates the camera in the board plane around its viewpoint whilst
        //! constraining it to be within its bounding sphere.
        void rotate( const solosnake::radians r ) override;

        //! Moves the camera parallel to the board plane. Translates the
        //! lookedat point by a similar x and z.
        void translate( const float x, const float z ) override;

        //! Moves the camera towards or away from its lookedat point.
        //! Zooms in/out. Zoom factor is applied to Amount.
        void move_towards_lookat( const float Amount ) override;

        //! Sets the current view and projection matrices into the rendering system.
        //! This should be called once before rendering the scene. This will take into account
        //! any update to the window size etc.
        void setup_scene_view_and_proj_matrices();

        solosnake::point3d get_boardplanepick( solosnake::screenxy xy ) const;

        pickresult get_boardpick( solosnake::screenxy xy, const Hex_grid& board ) const;

    private:

        solosnake::rendering_system* update_view_and_proj();

    private:

        std::unique_ptr<iboardcamera>                   camera_;
        std::shared_ptr<solosnake::rendering_system>    rendering_;
        solosnake::matrix4x4_t                          proj_;
        solosnake::matrix4x4_t                          view_;
        float                                           zNear_;
        float                                           zFar_;
        solosnake::radians                              viewAngle_;
    };
}

#endif
