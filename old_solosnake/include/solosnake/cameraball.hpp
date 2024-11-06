#ifndef solosnake_cameraball_hpp
#define solosnake_cameraball_hpp

#include "solosnake/box.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/point.hpp"

namespace solosnake
{
    //! "Ball" shaped camera that can be restricted to be within a bounding
    //! axis aligned rectangle. The camera will never move the ball outside
    //! the box interior and no operation on the camera will cause it
    //! to leave the box.
    class cameraball
    {
    public:

        cameraball();

        cameraball( float radius, const point3d& startingPosition, const box& boundingBox );

        const float* location() const
        {
            return location_.xyz;
        }

        //! Camera's up local 3d up vector.
        const float* up() const
        {
            return up_.xyz;
        }

        //! 3d vector camera is pointing along.
        const float* forwards() const
        {
            return forwards_.xyz;
        }

        float radius() const
        {
            return radius_;
        }

        //! Positions the camera to look at @a p
        //! Moves the camera instantly to be looking at this point, moving the
        //! camera backwards if the location is nearer than the radius.
        //! If moving the camera backwards to be "radius" away from the lookat
        //! point would move the camera outside its rectangle, then the camera
        //! will not look at the requested xyz but will look along the line
        //! through xyz and past it.
        void lookat( const point3d& p );

        //! Moves camera forwards, negative moves its backwards.
        void move_forwards( float nUnits );

        //! Moves the camera to the nearest point in the box to @a p.
        void move_to( const point3d& p );

        //! Moves the camera by this amount but will not allow it to leave its
        //! bounding rectangle.
        void translate( float x, float y, float z );

        //! Creates in m4x4 a view matrix modelling the camera.
        void set_into_view_matrix( float* unaliased m4x4 ) const;

    private:
        point3d location_;
        point3d forwards_;
        point3d up_;
        float   radius_;
        box     boundingBox_;
    };
}

#endif
