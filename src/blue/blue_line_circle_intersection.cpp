#include "solosnake/blue/blue_circle.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_intersection2d.hpp"
#include "solosnake/blue/blue_line2d.hpp"
#include "solosnake/blue/blue_line_circle_intersection.hpp"

namespace blue
{
    namespace
    {
        constexpr float HALF_PI_RADIANS{0.50f * BLUE_PI};

        inline float sq( float f ) noexcept
        {
            return f * f;
        }

        inline bool almost_zero( float f ) noexcept
        {
            return std::abs( f ) < 1e-10f;
        }

        struct solution
        {
            unsigned int solutions;
            float x[2];
        };

        solution solve( float A, float B, float C )
        {
            solution result = { 0, { 0.0f, 0.0f } };

            const float b2m4ac = ( B * B ) - ( 4.0f * A * C );

            if( b2m4ac >= 0.0f && ( false == almost_zero( A ) ) )
            {
                const float s = sqrt( b2m4ac );
                result.x[0] = ( -B + s ) / ( 2.0f * A );
                result.x[1] = ( -B - s ) / ( 2.0f * A );
                result.solutions = result.x[0] == result.x[1] ? 1 : 2;
            }

            return result;
        }

        /// Special case for A = 1
        solution solve( float B, float C )
        {
            solution result = { 0, { 0.0f, 0.0f } };

            const float b2m4ac = ( B * B ) - ( 4.0f * C );

            if( b2m4ac >= 0.0f )
            {
                const float s = sqrt( b2m4ac );
                result.x[0] = ( -B + s ) * 0.5f;
                result.x[1] = ( -B - s ) * 0.5f;
                result.solutions = result.x[0] == result.x[1] ? 1 : 2;
            }

            return result;
        }

        Intersection2d y_axis_parallel_line_circle_intersection( const Line2d& L, const Circle& C )
        {
            // Handle special case lines x = c
            float a = C.centre()[0];
            float b = C.centre()[1];
            float c = L.point_on_line()[0];
            float a2 = sq( a );
            float b2 = sq( b );
            float c2 = sq( c );
            float r2 = sq( C.radius() );

            solution s = solve( -2.0f * b, a2 + b2 + c2 - r2 - ( 2.0f * a * c ) );

            if( s.solutions == 2 )
            {
                return Intersection2d( c, s.x[0], c, s.x[1] );
            }
            else if( s.solutions == 1 )
            {
                return Intersection2d( c, s.x[0] );
            }
            else
            {
                // No solutions found.
                return Intersection2d();
            }
        }

    } // Unnamed.

    Intersection2d line_circle_intersection( const Line2d& L, const Circle& C )
    {
        if( L.angle() != HALF_PI_RADIANS )
        {
            float m = L.angle().tan();
            float a = C.centre()[0];
            float b = C.centre()[1];
            float c = L.point_on_line()[1] - m * L.point_on_line()[0];
            float d = c - b;

            float A1 = 1.0f + sq( m );
            float A2 = 2.0f * ( m * d - a );
            float A3 = sq( a ) + sq( d ) - sq( C.radius() );

            solution s = solve( A1, A2, A3 );

            if( s.solutions == 2 )
            {
                return Intersection2d( s.x[0], m * s.x[0] + c, s.x[1], m * s.x[1] + c );
            }
            else if( s.solutions == 1 )
            {
                return Intersection2d( s.x[0], m * s.x[0] + c );
            }
        }
        else
        {
            // Special case x = c line.
            return y_axis_parallel_line_circle_intersection( L, C );
        }

        // No solutions found.
        return Intersection2d();
    }
}
