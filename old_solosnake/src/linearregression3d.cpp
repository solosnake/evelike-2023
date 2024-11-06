#include "solosnake/linearregression3d.hpp"
#include <cmath>
#include <stdexcept>

namespace solosnake
{
    namespace linear_regression
    {
        namespace
        {
            static const double Pi = 3.1415926535897932384626433832795;

            inline double sqr( double d )
            {
                return d * d;
            }

            inline double cubed( double d )
            {
                return d * d * d;
            }

            inline double arctan( double d )
            {
                return std::atan( d );
            }

            inline double arccos( double d )
            {
                return std::acos( d );
            }

            inline double root2( double d )
            {
                return std::sqrt( d );
            }

            inline double root3( double d )
            {
                throw std::runtime_error("NOT IMPLEMENTED");
            }
        }

        bool try_get_linear_regression( const std::vector<point3d>& points, line3d& users )
        {
            if( points.size() < 2 )
            {
                return false;
            }

            double Xm = 0.0;
            double Ym = 0.0;
            double Zm = 0.0;

            // temps
            double xy = 0.0;
            double xz = 0.0;
            double yz = 0.0;
            double X2 = 0.0;
            double Y2 = 0.0;
            double Z2 = 0.0;

            for( size_t i = 0; i < points.size(); ++i )
            {
                const double x = points[i].x;
                const double y = points[i].y;
                const double z = points[i].z;

                Xm += x;
                xy += x * y;
                xz += x * z;
                X2 += x * x;
                Ym += y;
                Y2 += y * y;
                yz += y * z;
                Zm += z;
                Z2 += z * z;
            }

            const double n = static_cast<double>( points.size() );

            Xm /= n;
            Ym /= n;
            Zm /= n;

            //

            const double Sxx = -sqr( Xm ) + X2 / n;

            const double Syy = -sqr( Ym ) + Y2 / n;

            const double Szz = -sqr( Zm ) + Z2 / n;

            const double Sxy = -Xm * Ym + xy / n;

            const double Sxz = -Xm * Zm + xz / n;

            const double Syz = -( Ym * Zm ) + ( yz / n );

            //

            const double omega = 0.5 * arctan( ( 2.0 * Sxy ) / ( Sxx - Syy ) );

            //

            const double cos0 = std::cos( omega );
            const double sin0 = std::sin( omega );

            const double cos20 = sqr( std::cos( omega ) );
            const double sin20 = sqr( std::sin( omega ) );

            //

            const double K11 = ( Syy + Szz ) * cos20 + ( Sxx + Szz ) * sin20 - 2.0 * Sxy * cos0 * sin0;

            const double K22 = ( Syy + Szz ) * sin20 + ( Sxx + Szz ) * cos20 + 2.0 * Sxy * cos0 * sin0;

            // Although mentioned in the notes
            // (http://www.scribd.com/doc/31477970/Regressions-et-trajectoires-3D)
            // this
            // value is not actually used in the calculations.
            // const double K12 = - Sxy * (cos20 - sin20)  +  (Sxx - Syy) * cos0 *
            // sin0;

            const double K10 = Sxz * cos0 + Syz * sin0;

            const double K01 = -Sxz * sin0 + Syz * cos0;

            const double K00 = Sxx + Syy;

            //

            const double c2 = -K00 - K11 - K22;

            const double c1 = K00 * K11 + K00 * K22 + K11 * K22 - sqr( K01 ) - sqr( K10 );

            const double c0 = sqr( K01 ) * K11 + sqr( K10 ) * K22 - K00 * K11 * K22;

            //

            const double p = c1 - sqr( c2 ) / 3.0;

            const double q = ( 2.0 / 27.0 ) * cubed( c2 ) - ( ( c1 * c2 ) / 3.0 ) + c0;

            const double R = 0.25 * sqr( q ) + cubed( p ) / 27.0;

            //

            double rho2 = 0.0;

            if( R > 0.0 )
            {
                rho2 = -c2 / 3.0 + root3( -0.5 * q + root2( R ) ) + root3( -0.5 * q - root2( R ) );
            }
            else if( R < 0.0 )
            {
                const double phi = root2( -cubed( p ) / 27.0 );

                const double gamma = arccos( -q / ( 2.0 * phi ) );

                const double rho2a = -c2 / 3.0 + 2.0 * root3( phi ) * std::cos( gamma / 3.0 );

                const double rho2b = -c2 / 3.0 + 2.0 * root3( phi ) * std::cos( ( gamma + 2.0 * Pi ) / 3.0 );

                const double rho2c = -c2 / 3.0 + 2.0 * root3( phi ) * std::cos( ( gamma + 4.0 * Pi ) / 3.0 );

                // find lowest of three
                rho2 = std::min( rho2a, std::min( rho2b, rho2c ) );
            }
            else
            {
                return false;
            }

            //

            const double a = -( K10 * cos0 ) / ( K11 - rho2 ) + ( K01 * sin0 ) / ( K22 - rho2 );

            const double b = -( K10 * sin0 ) / ( K11 - rho2 ) - ( K01 * cos0 ) / ( K22 - rho2 );

            //

            const double u = ( 1.0 / ( 1.0 + sqr( a ) + sqr( b ) ) )
                             * ( ( 1.0 + sqr( b ) ) * Xm - ( a * b * Ym ) + ( a * Zm ) );

            const double v = ( 1.0 / ( 1.0 + sqr( a ) + sqr( b ) ) )
                             * ( ( -a * b * Xm ) + ( ( 1.0 + sqr( a ) ) * Ym ) + b * Zm );

            const double w = ( 1.0 / ( 1.0 + sqr( a ) + sqr( b ) ) )
                             * ( ( a * Xm ) + ( b * Ym ) + ( ( sqr( a ) + sqr( b ) ) * Zm ) );

            users.Xm = Xm;
            users.Ym = Ym;
            users.Zm = Zm;

            users.Xh = u;
            users.Yh = v;
            users.Zh = w;

            return true;
        }
    }
}
