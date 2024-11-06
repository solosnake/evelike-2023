#ifndef solosnake_iworldobject_hpp
#define solosnake_iworldobject_hpp

#include "solosnake/point.hpp"
#include "solosnake/matrix3d.hpp"
#include <memory>

namespace solosnake
{
    //! Base class for a 3D world based object. This class is quite unprotected
    //! in terms of what can be done with it. Care should be taken to maintain
    //! that the orientation matrix is a valid orthogonal basis matrix.
    //! In OpenGL by default the forward matrix is 0,0,1, and left is 1,0,0.
    //! @note This class is used by the sound renderer world objects.
    class iworldobject
    {
    public:
        iworldobject();

        explicit iworldobject( const float* m4x4 );

        virtual ~iworldobject();

        bool is_valid() const;

        point3d up() const;

        point3d down() const;

        point3d left() const;

        point3d right() const;

        point3d forward() const;

        point3d backward() const;

        point3d location() const;

        point3d dirvector() const;

        //! Measured in world units per second.
        point3d velocity() const;

        //! Measured in world units per second.
        float speed() const;

        void make_invalid();

        const float* matrix() const
        {
            return orientation_;
        }

        void set_direction_and_speed( point3d, float speed );

        void set_location( point3d );

        void set_xz( float x, float z );

    protected:
        point4_t dirvector_;      // x,y,z,speed.
        matrix4x4_t orientation_; // Position and direction.
    };

    typedef std::shared_ptr<iworldobject> iworldobject_ptr;

    //-------------------------------------------------------------------------

    inline bool iworldobject::is_valid() const
    {
        return orientation_[15] != 0.0f;
    }

    inline void iworldobject::make_invalid()
    {
        orientation_[15] = 0.0f;
    }

    inline point3d iworldobject::up() const
    {
        return solosnake::up_4x4( orientation_ );
    }

    inline point3d iworldobject::down() const
    {
        return solosnake::down_4x4( orientation_ );
    }

    inline point3d iworldobject::left() const
    {
        return solosnake::left_4x4( orientation_ );
    }

    inline point3d iworldobject::right() const
    {
        return solosnake::right_4x4( orientation_ );
    }

    inline point3d iworldobject::forward() const
    {
        return solosnake::forward_4x4( orientation_ );
    }

    inline point3d iworldobject::backward() const
    {
        return solosnake::backward_4x4( orientation_ );
    }

    inline point3d iworldobject::location() const
    {
        return solosnake::location_4x4( orientation_ );
    }

    inline point3d iworldobject::dirvector() const
    {
        return point3d( dirvector_[0], dirvector_[1], dirvector_[2] );
    }

    inline point3d iworldobject::velocity() const
    {
        return point3d( dirvector_[0] * dirvector_[3],
                        dirvector_[1] * dirvector_[3],
                        dirvector_[2] * dirvector_[3] );
    }

    //! Measured in world units per second.
    inline float iworldobject::speed() const
    {
        return dirvector_[3];
    }
}

#endif
