#ifndef solosnake_box_hpp
#define solosnake_box_hpp

#include "solosnake/point.hpp"
#include "solosnake/dimension.hpp"

namespace solosnake
{
    class box
    {
    public:

        //! Zero dimension box at 0,0,0.
        box();

        box( const Point3d& corner, const dimension3df& size );

        //! Returns true if @a p is inside box.
        bool contains( const Point3d& p ) const;

        //! Returns the nearest point in or on the box surface to @a p. */
        Point3d nearest_point_to( const Point3d& p ) const;

        //! Returns mid point in box.
        Point3d centre() const;

        //! Returns a copy of this box shrunk towards its centre by this amount.
        //! Each side of the new box will have been moved 50% of this amount
        //! towards the centre.
        box shrunk_by( float amount ) const;

    private:

        Point3d      corner_;
        dimension3df size_;
    };

    //-------------------------------------------------------------------------

    inline Point3d box::centre() const
    {
        return Point3d(
                   corner_.xyz[0] + size_.x() * 0.5f,
                   corner_.xyz[1] + size_.y() * 0.5f,
                   corner_.xyz[2] + size_.z() * 0.5f );
    }
}

#endif
