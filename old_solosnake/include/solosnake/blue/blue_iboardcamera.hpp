#ifndef blue_iboardcamera_hpp
#define blue_iboardcamera_hpp

#include "solosnake/point.hpp"
#include "solosnake/radians.hpp"

namespace blue
{
    //! Base class for board cameras.
    class iboardcamera
    {
    public:

        virtual ~iboardcamera();

        virtual float min_distance_to_lookat() const = 0;

        virtual float max_distance_to_lookat() const = 0;

        virtual solosnake::point3d location() const = 0;

        virtual solosnake::point3d upwards() const = 0;

        virtual solosnake::point3d forwards() const = 0;

        virtual solosnake::point3d lookedat() const = 0;

        //! Sets into the 4x4 matrix the view matrix representing
        //! the current camera's position and orientation.
        virtual void get_view_matrix(
            float* unaliased m4x4 ) const = 0;

        //! Sets up side and ahead 3d vectors.
        virtual void get_orientation(
            float* unaliased up,
            float* unaliased side,
            float* unaliased ahead ) const = 0;

        //! Moves the camera parallel to the board plane.
        //! Translates the lookedat point by dx and dz.
        virtual void translate( const float dx, const float dz ) = 0;

        //! Moves the camera towards or away from its lookedat point.
        //! Zooms in/out. Zoom factor is applied to amount.
        virtual void move_towards_lookat( const float amount ) = 0;

        //! Rotates the camera around the up axis of its viewpoint.
        virtual void rotate( const solosnake::radians r ) = 0;
    };
}

#endif
