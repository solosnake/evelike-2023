#ifndef solosnake_LINEAR_REGRESSION3D_HPP
#define solosnake_LINEAR_REGRESSION3D_HPP

#include <vector>

// With thanks to "Regressions et Trajectoires en 3D" by Jean Jacquelin.

namespace solosnake
{
    namespace linear_regression
    {
        //! A line MH which is the 3D linear least squares fit line for a cloud
        //! of points, the best fitting straight line through a set of points.
        struct line3d
        {
            double Xm;
            double Ym;
            double Zm;

            double Xh;
            double Yh;
            double Zh;
        };

        struct point3d
        {
            point3d() : x(), y(), z()
            {
            }

            point3d( double i, double j, double k ) : x( i ), y( j ), z( k )
            {
            }

            double x;
            double y;
            double z;
        };

        //! Tries to find and fill in the best fitting straight line for the
        //! given set of points. Returns true if a line was found, filling in
        //! the user's data, returns false otherwise.
        bool try_get_linear_regression( const std::vector<point3d>&, line3d& );
    }
}

#endif
