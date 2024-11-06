#include <cmath>
#include <stack>
#include "solosnake/blue/blue_circle.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_half_line_3d.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_hex_steps.hpp"
#include "solosnake/blue/blue_intersection2d.hpp"
#include "solosnake/blue/blue_line2d.hpp"
#include "solosnake/blue/blue_line_circle_intersection.hpp"
#include "solosnake/blue/blue_point2d.hpp"
#include "solosnake/blue/blue_rect.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    // These are not too important, just checking some decisions were sound:
    static_assert( sizeof( Hex_grid::Neighbouring_hex_tile ) == 4,
                   "Size of Neighbouring_hex_tile is not what was expected." );

    namespace
    {
        //         _ _ _ _ _
        //        /         \      | H
        //       /           \     | e
        //      /             \    | i
        //      \             /    | g
        //       \           /     | h
        //        \_ _ _ _ _/      | t
        //      |- W i d t h -|
        //         |- Face -|
        //
        static const float HexWidth           = 4.0f / std::sqrt( 3.0f );   // ~2.309
        static const float HexHalfWidth       = 2.0f / std::sqrt( 3.0f );   // ~1.155
        static const float HexQuarterWidth    = 1.0f / std::sqrt( 3.0f );   // ~0.577
        static const float HexHalfHeight      = 1.0f;

        static const float HexHalfFaceWidth   = 1.0f / std::sqrt( 3.0f );   // ~0.577
        static const float HexHalfFaceWidthSq = 1.0f / 3.0f;                // ~0.333

        static const float HexMidFaceX        = std::sqrt( 3.0f ) / 2.0f;   // ~0.866
        static const float HexMidFaceY        = 0.5f;

        /// Midpoints of the faces of a HexWidth hex centred at 0,0.
        static const float FaceMidPoints[7][2] =
        {
            {         0.0f, -HexHalfHeight },
            {  HexMidFaceX, -HexMidFaceY },
            {  HexMidFaceX, +HexMidFaceY },
            {         0.0f, +HexHalfHeight },
            { -HexMidFaceX, +HexMidFaceY },
            { -HexMidFaceX, -HexMidFaceY },
            {         0.0f, -HexHalfHeight } // Repeat point 0, for algorithm.
        };

        /// Axial neighbours ordered by facing
        static const std::int8_t FacingNeighbours[6][2] =
        {
            { +0, -1 }, { +1, -1 }, { +1, +0 },
            { +0, +1 }, { -1, +1 }, { -1, +0 }
        };

        /// Offsets to tiles reached through the corner points, ordered by facing.
        static const std::int8_t CornerNeighbours[7][2] =
        {
            { +1, +1 }, { +2, -1 }, { +1, -2 },
            { -1, -1 }, { -2, +1 }, { -1, +2 },
            { +1, +1 }
        };

        /// -1 represents an undefined neighbour. Only 6 of the possible 9
        /// combinations are valid.
        static const std::int8_t FacingNeighbourNames[3][3] =
        {
            { -1, FacingTile5, FacingTile4 },
            { FacingTile0, -1, FacingTile3 },
            { FacingTile1, FacingTile2, -1 }
        };

        /// Returns the facing quadrant that the given angle is within.
        static inline HexFacingName closest_facing_from_real_angle( Radians angle )
        {
            assert( angle.value() >= 0.0f && angle.value() <= BLUE_TWO_PI );

            float a = angle.value() + BLUE_PI / 6.0f; // Same as 2 Pi / 12
            a = a > BLUE_TWO_PI ? a - BLUE_TWO_PI : a;

            assert( a >= 0.0f && a <= BLUE_TWO_PI );

            std::uint16_t facing = 0;

            while( a > ( ( 1 + facing ) * ( BLUE_PI / 3.0f ) ) )
            {
                // Its not in this quadrant, try the next:
                ++facing;
            }

            assert( facing < 6 );

            // Now adjust for the fact that facing 2 is 0 in radians:
            return static_cast<HexFacingName>( ( facing + 2 ) % 6 );
        }

        static uint8_t validate_board_width( const unsigned int w )
        {
            if( w > Hex_grid::MaxPermittedGridWidth )
            {
                ss_throw( "Requested board width too large." );
            }

            if( w == 0 )
            {
                ss_throw( "Requested board width must not be zero." );
            }

            return static_cast<uint8_t>( w );
        }

        static uint8_t validate_board_height( const unsigned int h )
        {
            if( h > Hex_grid::MaxPermittedGridHeight )
            {
                ss_throw( "Requested board height too large." );
            }

            if( h == 0 )
            {
                ss_throw( "Requested board height must not be zero." );
            }

            return static_cast<uint8_t>( h );
        }

        inline float length2( float const * const unaliased p )
        {
            return std::sqrt( ( p[0] * p[0] ) + ( p[1] * p[1] ) );
        }

    } // Anonymous

    Hex_grid::Hex_grid( const unsigned int w,
                        const unsigned int h,
                        const HexContentFlags defaultFlags,
                        const HexContentFlags invalidFlags )
        : grid_( validate_board_width( w ) * validate_board_height( h ), defaultFlags )
        , width_( static_cast<int8_t>( w ) )
        , height_( static_cast<int8_t>( h ) )
        , invalid_tile_value_( invalidFlags )
    {
        assert( invalidFlags != 0 );
    }

    Hex_grid::Hex_grid( std::vector<HexContentFlags>&& grid,
                        const unsigned int width,
                        const unsigned int height,
                        const HexContentFlags invalidTile )
        : grid_( std::move(grid) )
        , width_( validate_board_width( width ) )
        , height_( validate_board_height( height ) )
        , invalid_tile_value_( invalidTile )
    {
        if( grid_.size() != static_cast<std::size_t>(width_ * height_) )
        {
            ss_throw("Grid and width/height do not match.");
        }

        if( invalidTile == 0 )
        {
            ss_throw( "Invalid tile value is not permitted to be zero.");
        }
    }

    Hex_coord Hex_grid::calculate_coord( const float x, const float y )
    {
        // Find hex closest to coordinates, and see which of it or its neighbours
        // are closest to the clicked point. By Voronoi regions x,y is in
        // that hex.

        // Axial coordinates for x,y.
        const float ax = x / SS_SQRT_3;
        const float ay = 0.5f * ( y - x / SS_SQRT_3 );

        // Get estimate of a nearby hex centre in axial coordinates:
        const int fx = static_cast<int>( std::floor( ax + 0.5f ) );
        const int fy = static_cast<int>( std::floor( ay + 0.5f ) );

        // Centre & Axial neighbours offsets:
        const int offsets[7][2] =
        {
            { +0, +0 }, { +1, +0 }, { +1, -1 },
            { +0, -1 }, { -1, +0 }, { -1, +1 },
            { +0, +1 }
        };

        unsigned int closest = 0xFFFF;
        float closesDistance = 0xFFFF;

        // Loop and find the closest hex centre to the clicked point.
        for( unsigned int i = 0; i < 7; ++i )
        {
            // Convert from axial hexcoord to board location:
            const float hx = ( fx + offsets[i][0] ) * SS_SQRT_3;
            const float hy = 2.0f * ( fy + offsets[i][1] ) + ( fx + offsets[i][0] );

            const float dx = x - hx;
            const float dy = y - hy;

            const float sqDistanceToHexCentre = ( dx * dx ) + ( dy * dy );

            if( closesDistance > sqDistanceToHexCentre )
            {
                closesDistance = sqDistanceToHexCentre;
                closest = i;
            }
        }

        assert( closest < 7 );
        return Hex_coord::try_make_coord_from_axial( fx + offsets[closest][0],
                                                    fy + offsets[closest][1] );
    }

    unsigned int Hex_grid::count_valid_tiles() const
    {
        unsigned int n = 0;

        for( unsigned int i = 0; i < grid_.size(); ++i )
        {
            if( 0 == ( grid_[i] & invalid_tile_value_ ) )
            {
                ++n;
            }
        }

        return n;
    }

    HexContentFlags Hex_grid::get_neighbour_contents( Hex_coord c, const HexFacingName facing ) const noexcept
    {
        // c will be altered here...
        if( try_get_neighbour_tile( c, facing, c ) )
        {
            return contents( c );
        }

        return invalid_tile_value_;
    }

    Rectf Hex_grid::surface_rect() const
    {
        const Hex_coord origin = Hex_coord::make_coord( 0, 0 );
        const Hex_coord farCorner = Hex_coord::make_coord( width_ - 1, height_ - 1 );

        float originXY[2];
        float farCornerXY[2];

        calculate_xy( origin, originXY );
        calculate_xy( farCorner, farCornerXY );

        const float w = HexHalfWidth + farCornerXY[0] - originXY[0];
        const float h = HexHalfHeight + farCornerXY[1] - originXY[1];

        return Rectf( -HexHalfWidth, -HexHalfHeight, w, h );
    }

    float Hex_grid::get_short_side_3d_box_diagonal() const
    {
        // For now lets take the board as a box, as high as its shortest side.
        const float boxW = static_cast<float>( grid_width() );
        const float boxL = static_cast<float>( grid_height() );
        const float boxDiagonal = std::sqrt( ( boxW * boxW ) + ( boxL * boxL ) );
        const float boxSpan     = std::sqrt( ( boxDiagonal * boxDiagonal ) + std::max( boxW, boxL ) );

        return boxSpan;
    }

    unsigned int Hex_grid::count_of_hexagons_inside_ring( const unsigned int n )
    {
        // Store a pre-calculated set of lookups.
        const unsigned int lookups[6] = { 1, 7, 19, 37, 61, 91 };

        unsigned int tile_count = ( n < 6 ) ? lookups[n] : 91;

        // If n is outside range of pre-calculated, calculate it.
        for( unsigned int i = 5; i < n; ++i )
        {
            tile_count += i * 6 + 6;
        }

        return tile_count;
    }

    void Hex_grid::get_sorted_circle_contents( const Hex_coord c,
                                               const std::uint16_t r,
                                               std::vector<Hex_tile>& tiles,
                                               HexContentFlags mask ) const
    {
        // Arbitrary check on sanity.
        assert( r < 100 );

        tiles.clear();

        if( is_in_board_bounds( c ) )
        {
            Hex_tile h;
            h.tile_contents = contents( c );
            if( 0 == mask || h.tile_contents & mask )
            {
                h.tile_coordinate = c;
                tiles.push_back( h );
            }

            // Axial neighbours:
            const int NeighbourOffsets[6][2] =
            {
                { +1, +0 }, { +1, -1 }, { +0, -1 },
                { -1, +0 }, { -1, +1 }, { +0, +1 }
            };

            const int radius = static_cast<int>( r );
            for( int ring = 1; ring <= radius; ++ring )
            {
                // Move in direction of neighbour #4, { -1, +1 }
                // nx and ny are axial coordinates updated inside loop.
                int nx = -ring + SS_AXIAL_X_FROM_COORD( c.x, c.y );
                int ny =  ring + SS_AXIAL_Y_FROM_COORD( c.x, c.y );

                for( unsigned int i = 0; i < 6; ++i )
                {
                    // Gather up all tiles along a straight line the
                    // same length as the ring radius.
                    for( int j = 0; j < ring; ++j )
                    {
                        // Get hex coord from current axial location.
                        const int cx = SS_COORD_X_FROM_AXIAL( nx, ny );
                        const int cy = SS_COORD_Y_FROM_AXIAL( nx, ny );

                        // Check if hex coord is inside board grid.
                        if( cx >= 0 && cx < width_ && cy >= 0 && cy < height_ )
                        {
                            h.tile_contents = grid_[width_ * cy + cx];

                            if( 0 == mask || h.tile_contents & mask )
                            {
                                h.tile_coordinate.x = static_cast<int8_t>( cx );
                                h.tile_coordinate.y = static_cast<int8_t>( cy );
                                tiles.push_back( h );
                            }
                        }

                        // Move to next axial coordinate along straight line.
                        nx += NeighbourOffsets[i][0];
                        ny += NeighbourOffsets[i][1];
                    }
                }
            }
        }
    }

    Hex_coord Hex_grid::calculate_neighbour_coord( const Hex_coord c,
                                                   const HexFacingName facing,
                                                   const int n )
    {
        // Neighbour's axial coordinates:
        const int ax = n * FacingNeighbours[facing][0] + SS_AXIAL_X_FROM_COORD( c.x, c.y );
        const int az = n * FacingNeighbours[facing][1] + SS_AXIAL_Y_FROM_COORD( c.x, c.y );

        return Hex_coord::from_axial( ax, az );
    }

    Hex_coord Hex_grid::facing_neighbour_coord( const Hex_coord c, const HexFacingName facing )
    {
        // Neighbour's axial coordinates:
        const int ax = FacingNeighbours[facing][0] + SS_AXIAL_X_FROM_COORD( c.x, c.y );
        const int az = FacingNeighbours[facing][1] + SS_AXIAL_Y_FROM_COORD( c.x, c.y );

        return Hex_coord::from_axial( ax, az );
    }

    HexFacingName Hex_grid::neighbors_facing( const Hex_coord c, const Hex_coord neighbour )
    {
        assert( get_step_distance_between( c, neighbour ) == 1 );

        const int cx = SS_AXIAL_X_FROM_COORD( c.x, c.y );
        const int cz = SS_AXIAL_Y_FROM_COORD( c.x, c.y );

        const int nx = SS_AXIAL_X_FROM_COORD( neighbour.x, neighbour.y );
        const int nz = SS_AXIAL_Y_FROM_COORD( neighbour.x, neighbour.y );

        // Add +1 to both to bring to 0 .. 2 range from -1 .. +1 range for indexing.
        const int dx = 1 + nx - cx;
        const int dz = 1 + nz - cz;

        return static_cast<HexFacingName>( FacingNeighbourNames[dx][dz] );
    }

    void Hex_grid::contents_under_line( const Hex_coord A,
                                        const Hex_coord B,
                                        const std::uint16_t maxSteps,
                                        const HexContentFlags mask,
                                        std::vector<Hex_coord>& v ) const
    {
        v.clear();

        auto fn = [&]( int x, int y ) -> bool
        {
            const auto current_hex   = Hex_coord::make_coord( x, y );
            const auto step_distance = Hex_grid::get_step_distance_between( current_hex, A );
            const bool in_range      = step_distance <= maxSteps;

            if( in_range && is_in_board_bounds( current_hex ) )
            {
                if( ( mask == 0 ) || ( mask & contents( current_hex ) ) )
                {
                    v.push_back( current_hex );
                }
            }

            return in_range;
        };

        hex_steps_under_line( A.x, A.y, B.x, B.y, fn );
    }

    bool Hex_grid::try_get_neighbour_tile( const Hex_coord c,
                                           const HexFacingName facing,
                                           Hex_coord& n ) const noexcept
    {
        bool result = is_in_board_bounds( c );

        if( result )
        {
            // Neighbour's axial coordinates:
            const int ax = FacingNeighbours[facing][0] + SS_AXIAL_X_FROM_COORD( c.x, c.y );
            const int az = FacingNeighbours[facing][1] + SS_AXIAL_Y_FROM_COORD( c.x, c.y );

            // And back to odd-q
            const int nx = SS_COORD_X_FROM_AXIAL( ax, az );
            const int ny = SS_COORD_Y_FROM_AXIAL( ax, az );

            // Is it inside board grid?
            result = nx >= 0 && nx < width_ && ny >= 0 && ny < height_;

            if( result )
            {
                // n changed only when inside.
                n.x = static_cast<int8_t>( nx );
                n.y = static_cast<int8_t>( ny );
            }
        }

        return result;
    }

    Hex_grid::Neighbours Hex_grid::get_neighbours( const Hex_coord c,
                                                   const HexContentFlags mask ) const noexcept
    {
        Hex_grid::Neighbours n{};
        n.size_ = 0;
        n.centre_.tile_contents = invalid_tile_value_;
        n.centre_.tile_coordinate = c;

        if( is_in_board_bounds( c ) )
        {
            n.centre_.tile_contents = grid_[width_ * c.y + c.x];

            for( std::int8_t i = 0; i < 6; ++i )
            {
                Hex_coord nbr = facing_neighbour_coord( c, static_cast<HexFacingName>( i ) );

                if( is_in_board_bounds( nbr ) )
                {
                    const HexContentFlags content = contents( nbr );

                    if( 0 == mask || content & mask )
                    {
                        n.neighbours_[n.size_].tile_coordinate = nbr;
                        n.neighbours_[n.size_].tile_contents   = content;
                        n.neighbours_[n.size_].relative_facing = static_cast<std::int8_t>( i );
                        ++n.size_;
                    }
                }
            }
        }

        return n;
    }

    unsigned int Hex_grid::get_clockwise_turn_count( const Hex_coord xyFrom,
                                                     const HexFacingName facing,
                                                     const Hex_coord xyTo )
    {
        if( xyFrom != xyTo )
        {
            float fromWorldXY[2];
            float toWorldXY[2];
            calculate_xy( xyFrom, fromWorldXY );
            calculate_xy( xyTo, toWorldXY );

            // Make positions relative:
            toWorldXY[0] -= fromWorldXY[0];
            toWorldXY[1] -= fromWorldXY[1];

            // Convert to unit circle coord:
            const float d = length2( toWorldXY );
            toWorldXY[0] /= d;
            toWorldXY[1] /= d;

            // Range of atan2 is -pi to +pi. Convert to 0 to 2 pi.
            float angleTo = atan2( toWorldXY[1], toWorldXY[0] );
            angleTo = angleTo < 0.0f ? angleTo + BLUE_TWO_PI : angleTo;
            assert( angleTo >= 0.0f && angleTo <= BLUE_TWO_PI );

            const unsigned int facingWanted = closest_facing_from_real_angle( Radians( angleTo ) );
            const unsigned int facingNow = facing;

            assert( ( facingWanted == 0 || facingWanted > 0 ) && facingWanted <= 5 );
            assert( ( facingWanted >= facingNow ? ( facingWanted - facingNow )
                      : ( facingWanted + 6 - facingNow ) ) < 6 );

            return facingWanted >= facingNow
                ? ( facingWanted - facingNow )
                : ( facingWanted + 6 - facingNow );
        }
        else
        {
            // No turns when from == to.
            return 0;
        }
    }

    void Hex_grid::get_hexagon_vertices( const Hex_coord xy,
                                         const float scaleHexagon,
                                         float* unaliased fx3x6 )
    {
        constexpr auto board_y = 0.0f;

        float centre[2];
        calculate_xy( xy, centre );

        fx3x6[0] = scaleHexagon * HexQuarterWidth + centre[0];
        fx3x6[1] = board_y;
        fx3x6[2] = scaleHexagon * -HexHalfHeight + centre[1];

        fx3x6[3] = scaleHexagon * HexHalfWidth + centre[0];
        fx3x6[4] = board_y;
        fx3x6[5] = centre[1];

        fx3x6[6] = scaleHexagon * HexQuarterWidth + centre[0];
        fx3x6[7] = board_y;
        fx3x6[8] = scaleHexagon * HexHalfHeight + centre[1];

        fx3x6[9] = scaleHexagon * -HexQuarterWidth + centre[0];
        fx3x6[10] = board_y;
        fx3x6[11] = scaleHexagon * HexHalfHeight + centre[1];

        fx3x6[12] = scaleHexagon * -HexHalfWidth + centre[0];
        fx3x6[13] = board_y;
        fx3x6[14] = centre[1];

        fx3x6[15] = scaleHexagon * -HexQuarterWidth + centre[0];
        fx3x6[16] = board_y;
        fx3x6[17] = scaleHexagon * -HexHalfHeight + centre[1];
    }

    unsigned int Hex_grid::number_of_islands() const noexcept
    {
        const int h = grid_height();
        const int w = grid_width();
        uint16_t islands_count = 0;
        std::vector<uint16_t> visited_islands( grid_size(), uint16_t( 0u ) );
        std::stack<Hex_coord> island_tiles;

        auto mark_as_island = [&]( const Hex_coord c, const uint16_t id )
        {
            assert( island_tiles.empty() );

            island_tiles.push( c );
            while( ! island_tiles.empty() )
            {
                Hex_coord t = island_tiles.top();
                island_tiles.pop();

                if( visited_islands[t.x + t.y * w] == 0u )
                {
                    visited_islands[t.x + t.y * w] = id;

                    // To axial coordinates:
                    const int x = SS_AXIAL_X_FROM_COORD( t.x, t.y );
                    const int z = SS_AXIAL_Y_FROM_COORD( t.x, t.y );

                    for( unsigned int i = 0; i < 6u; ++i )
                    {
                        const int nx = x + FacingNeighbours[i][0];
                        const int nz = z + FacingNeighbours[i][1];
                        const Hex_coord n = Hex_coord::from_axial( nx, nz );
                        if( is_on_board( n ) )
                        {
                            if( 0u == visited_islands[n.x + n.y * w] )
                            {
                                island_tiles.push( n );
                            }
                            else
                            {
                                // We should only encounter unvisited or ourselves,
                                // never another island - as it would not be an
                                // 'island' if we could visit it.
                                assert( id == visited_islands[n.x + n.y * w] );
                            }
                        }
                    }
                }
            }
        };

        for( int j = 0; j < h; ++j )
        {
            for( int i = 0; i < w; ++i )
            {
                Hex_coord xy = Hex_coord::make_coord( static_cast<int8_t>( i ), static_cast<int8_t>( j ) );

                // For each unvisited tile on the board, mark it and all
                // its connected neighbours as being part of an island.
                if( is_on_board( xy ) && 0u == visited_islands[i + j * w] )
                {
                    ++islands_count;
                    mark_as_island( xy, islands_count );
                }
            }
        }

        return islands_count;
    }

    bool Hex_grid::operator == ( const Hex_grid& rhs ) const noexcept
    {
        return ( grid_width() == rhs.grid_width() )
            && ( grid_height() == rhs.grid_height() )
            && ( invalid_tile_value() == rhs.invalid_tile_value() )
            && ( grid_ == rhs.grid_ );
    }

    bool Hex_grid::operator != ( const Hex_grid& rhs ) const noexcept
    {
        return !( *this == rhs );
    }

    Hex_grid::Hex_tile Hex_grid::get_line_intersection( const Half_line_3d& line,
                                                        const float boardY ) const
    {
        Hex_grid::Hex_tile result;
        result.tile_contents   = invalid_tile_value();
        result.tile_coordinate = Hex_coord::make_invalid_coord();

        float intersectionXYZ[3];
        if( find_half_line_y_plane_intersection( line, boardY, intersectionXYZ ) )
        {
            ss_log( "World ", intersectionXYZ[0], " ", intersectionXYZ[1], " ", intersectionXYZ[2] );
            Hex_coord coords = Hex_grid::calculate_coord( intersectionXYZ[0], intersectionXYZ[2] );

            if( coords.is_valid() && is_in_board_bounds( coords ) )
            {
                result.tile_contents   = contents( coords );
                result.tile_coordinate = coords;
            }
        }

        return result;
    }

    void Hex_grid::set_contents_at( const Hex_coord c, const HexContentFlags flags )
    {
        if( is_in_board_bounds( c ) )
        {
            grid_.at(width_ * c.y + c.x) = flags;
        }
        else
        {
            ss_throw("Cannot set contents as hex coord is not in board.");
        }
    }

    HexContentFlags Hex_grid::contents_at( const Hex_coord c ) const
    {
        if( is_in_board_bounds( c ) )
        {
            return grid_.at(width_ * c.y + c.x);
        }
        else
        {
            ss_throw("Cannot get contents as hex coord is not in board.");
        }
    }
}
