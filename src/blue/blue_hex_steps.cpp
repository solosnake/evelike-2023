#include <cassert>
#include "solosnake/blue/blue_hex_steps.hpp"

namespace blue
{
    namespace
    {
        constexpr int x_dirs[2][2][3] =
        {
            { { -1, -1, +0 }, { -1, +1, +0 } },
            { { +0, -1, +1 }, { +0, +1, +1 } }
        };

        constexpr int y_dirs[2][2][3] =
        {
            { { -1, +0, -1 }, { +1, +0, +1 } },
            { { -1, +0, -1 }, { +1, +0, +1 } }
        };

        inline int dir_x( int i, int j, int k ) noexcept
        {
            i += 1;
            j = j < 0 ? 0 : 1;
            return x_dirs[k][j][i];
        }

        inline int dir_y( int i, int j, int k ) noexcept
        {
            i += 1;
            j = j < 0 ? 0 : 1;
            return y_dirs[k][j][i];
        }

        inline int odd_x_sign( int old_x ) noexcept
        {
            return old_x <=  0 ? -1 : +1;
        }

        inline int even_x_sign( int old_x ) noexcept
        {
            return old_x < 0 ? -1 : +1;
        }

        /// Does nothing of start and end are same points.
        void line_of_sight_on_hexagonal_grid( const int start_x,
                                              const int start_y,
                                              const int end_x,
                                              const int end_y,
                                              const std::function<bool( int, int )>& fn )
        {
            // This algorithm works with tiles in the opposite orientation:
            // the x axis of this algorithm is the hexgrid's y axis and vice versa.
            // For this reason the x/y are swapped.
            // See http://zvold.blogspot.se/2010/02/line-of-sight-on-hexagonal-grid.html

            if( start_x != end_x || start_y != end_y )
            {
                assert( ( end_y & 1 )   == ( end_y % 2 ) );
                assert( ( start_y & 1 ) == ( start_y % 2 ) );

                const int old_2x = 2 * ( end_x - start_x ) + std::abs( end_y & 1 ) - std::abs( start_y & 1 );
                const int old_x  = end_x - start_x;
                const int sign_x = ( start_y & 1 ) ? odd_x_sign( old_x ) : even_x_sign( old_x );
                const int dx    = std::abs( old_2x );

                const int old_y  = end_y - start_y;
                const int sign_y = old_y < 0 ? -1 : +1;
                const int dy    = std::abs( old_y );

                int current_x = start_x;
                int current_y = start_y;

                int e = -2 * dx;

                do
                {
                    if( e >= 0 )
                    {
                        e -= 3 * ( dy + dx );
                        current_x += dir_x( -sign_x, sign_y, current_y & 1 );
                        current_y += dir_y( -sign_x, sign_y, current_y & 1 );
                    }
                    else
                    {
                        e += 3 * dy;

                        if( e > -dx )
                        {
                            e -= 3 * dx;
                            current_x += dir_x( sign_x, sign_y, current_y & 1 );
                            current_y += dir_y( sign_x, sign_y, current_y & 1 );
                        }
                        else if( e < -3 * dx )
                        {
                            e += 3 * dx;
                            current_x += dir_x( sign_x, -sign_y, current_y & 1 );
                            current_y += dir_y( sign_x, -sign_y, current_y & 1 );
                        }
                        else
                        {
                            e += 3 * dy;
                            current_x += dir_x( 0, sign_x, current_y & 1 );
                            current_y += dir_y( 0, sign_x, current_y & 1 );
                        }
                    }
                }
                while( fn( current_y, current_x ) && ( current_x != end_x || current_y != end_y ) );
                // Note that we call back into fn with the x and y reversed.
            }
        }
    }

    /// Does nothing of start and end are same points.
    void hex_steps_under_line( const int start_x,
                               const int start_y,
                               const int end_x,
                               const int end_y,
                               const std::function<bool( int, int )>& fn )
    {
        // Swap x and y to accommodate original algorithm.
        line_of_sight_on_hexagonal_grid( start_y, start_x, end_y, end_x, fn );
    }
}
