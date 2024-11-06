#ifndef blue_gamecamera_hpp
#define blue_gamecamera_hpp

#include "solosnake/radians.hpp"
#include "solosnake/sphere.hpp"
#include "solosnake/blue/blue_iboardcamera.hpp"

namespace blue
{
    //! A board camera remains looking down at the horizontal board plane at all
    //! times. It can look straight down or horizontally across the board. It is
    //! modeled as a sphere, and is bounded by a box it must remain inside. The
    //! camera 'looks at' a point on the board raised 1 radius above the board
    //! plane.
    class gamecamera : public iboardcamera
    {
    public:

        gamecamera();

        gamecamera(
            const float boardY,
            const float radius,
            const float armlength,
            const float lookingatX,
            const float lookingatZ,
            const solosnake::radians& tilt,
            const solosnake::radians& mintilt,
            const solosnake::radians& maxtilt,
            const solosnake::radians& rotation,
            const solosnake::sphere& boundry,
            const float zoomFactor );

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
        //! Zooms in/out. Zoom factor is applied to amount.
        void move_towards_lookat( const float amount ) override;

    private:

        //! Radius of sphere camera is locked inside.
        float bounding_radius() const;

        //! Calculates location based on current lookat, tilt, rotation and
        //! armlength but does not move the camera.
        solosnake::point3d calculate_location() const;

    private:

        solosnake::sphere   boundingSphere_;
        solosnake::radians  tilt_;
        solosnake::radians  mintilt_;
        solosnake::radians  maxtilt_;
        solosnake::radians  rotation_;
        solosnake::point3d  lookedat_;
        float               armlength_;
        solosnake::point3d  location_;
        float               radius_;    // Maximum armlength.
        float               zoomfactor_;
    };
}

#include "solosnake/blue/blue_gamecamera.inl"

#endif
