#include "solosnake/iworldobject.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>

//!     From http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/
//!
//!     Any point in space can be represented by a linear combination of these vectors.
//!     m00     m01     m02
//!     m10     m11     m12
//!     m20     m21     m22
//!
//!     x y z = x * m00 m10 m20 + y * m01 m11 m21 + z * m02 m12 m22
//!
//!     where:
//!
//!     B1 = m00 m10 m20
//!
//!     B2= m01 m11 m21
//!
//!     B3 = m02 m12 m22
//!
//!     These basis vectors a mutually perpendicular, so the dot product of any two
//!     basis vectors is zero:
//!
//!     B1 . B2 = 0
//!     B2 . B3 = 0
//!     B3 . B1 = 0
//!
//!     Also, if we only know two basis vectors, we can derive the other by using vector
//!     cross multiplication, for instance:
//!
//!     B3 = B1 x B2
//!     B1 = B2 x B3
//!     B2 = B3 x B1
//!
//!     Another restriction on the values of the basis vectors is that they are of unit
//!     length.
//!
//!     |B1| = B1.x² + B1.y² + B1.z² = 1
//!     |B2| = B2.x² + B2.y² + B2.z² = 1
//!     |B3| = B3.x² + B3.y² + B3.z² = 1

namespace solosnake
{
    iworldobject::iworldobject()
    {
        dirvector_[0] = dirvector_[1] = dirvector_[2] = dirvector_[3] = 0.0f;
        load_identity_4x4( orientation_ );
    }

    iworldobject::iworldobject( const float* m4x4 )
    {
        dirvector_[0] = dirvector_[1] = dirvector_[2] = dirvector_[3] = 0.0f;
        load_copy_4x4( m4x4, orientation_ );
        assert( std::abs( rotation_determinant_4x4( m4x4 ) ) >= 0.98f
                && std::abs( rotation_determinant_4x4( m4x4 ) ) <= 1.02f );
    }

    iworldobject::~iworldobject()
    {
#ifndef NDEBUG
        ::memset( dirvector_, 0xCD, sizeof( dirvector_ ) );
        ::memset( orientation_, 0xCD, sizeof( orientation_ ) );
#endif
        make_invalid();
    }

    void iworldobject::set_direction_and_speed( point3d d, float speed )
    {
        dirvector_[0] = d[0];
        dirvector_[1] = d[1];
        dirvector_[2] = d[2];
        dirvector_[3] = speed;
    }

    void iworldobject::set_location( point3d p )
    {
        set_translation_4x4( p, orientation_ );
    }

    void iworldobject::set_xz( float x, float z )
    {
        point3d p = location_4x4( orientation_ );
        p[0] = x;
        p[2] = z;
        set_location( p );
    }
}
