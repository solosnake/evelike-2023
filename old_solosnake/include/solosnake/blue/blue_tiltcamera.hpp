#ifndef blue_tiltcamera_hpp
#define blue_tiltcamera_hpp

#include "solosnake/radians.hpp"
#include "solosnake/sphere.hpp"
#include "solosnake/blue/blue_iboardcamera.hpp"

namespace blue
{
    //! Camera that looks down on a board, but becomes more vertical as it
    //! moves away from the board. The tilt is a function of the
    //! distance to the looked-at point.
    class tiltcamera : public iboardcamera
    {
    public:

        tiltcamera();

        tiltcamera(
            const float boardY,
            const float radius,
            const float armlength,
            const float uprightArmlength,
            const float lookingatX,
            const float lookingatZ,
            const solosnake::radians mintilt,
            const solosnake::radians maxtilt,
            const solosnake::radians rotation,
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

      //! Tilt is a function of the arm length.
      solosnake::radians tilt() const;

    private:

      solosnake::sphere   boundingSphere_;
      solosnake::radians  mintilt_;
      solosnake::radians  maxtilt_;
      solosnake::radians  rotation_;
      solosnake::point3d  lookedat_;
      float               armlength_;
      float               upright_armlength_;
      solosnake::point3d  location_;
      float               radius_;    // Maximum armlength.
      float               zoomfactor_;
    };
}

#include "solosnake/blue/blue_tiltcamera.inl"

#endif
