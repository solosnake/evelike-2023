#include <utility>
#include "solosnake/blue/blue_attachpoint.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    namespace
    {
        constexpr auto BLUE_ATTACHPOINT_ROW_LEN = 10u;
        constexpr auto BLUE_MID_COORD           = 4u;

        bool positions_are_overlapping(
            const std::pair<unsigned int, unsigned int>& a,
            const std::pair<unsigned int, unsigned int>& b )
        {
            const unsigned int max_x = a.first >= b.first ? a.first : b.first;
            const unsigned int min_x = a.first >= b.first ? b.first : a.first;

            const unsigned int max_y = a.second >= b.second ? a.second : b.second;
            const unsigned int min_y = a.second >= b.second ? b.second : a.second;

            return ( max_x - min_x < 2 ) && ( max_y - min_y < 2 );
        }

        std::pair<unsigned int, unsigned int> attachpoint_xy( const unsigned short ap )
        {
            return std::pair<unsigned int, unsigned int>( ap / ( BLUE_ATTACHPOINT_ROW_LEN ),
                                                          ap % ( BLUE_ATTACHPOINT_ROW_LEN ) );
        }
    }

    bool is_location_number( const unsigned int n ) noexcept
    {
        return ( n  >  0u ) &&
               ( n  < 88u ) &&
               ( n !=  9u ) &&
               ( n != 19u ) &&
               ( n != 29u ) &&
               ( n != 39u ) &&
               ( n != 49u ) &&
               ( n != 59u ) &&
               ( n != 69u ) &&
               ( n != 79u );
    }

    bool is_location_number( const int n ) noexcept
    {
        return is_location_number( static_cast<unsigned int>( n ) );
    }

    AttachPoint::AttachPoint( const unsigned short n ) : location_number_( n )
    {
        if( ! is_location_number( n ) )
        {
            ss_throw( "Invalid AttachPoint." );
        }
    }

    bool AttachPoint::is_thruster_attachpoint() const noexcept
    {
        // Valid Thruster attach points are 81, 82, 83, 84, 85, 86 and 87.
        return ( location_number_ > 80u ) && ( location_number_ < 88u );
    }

    bool AttachPoint::conflicts_with( const AttachPoint& other ) const noexcept
    {
        return positions_are_overlapping( attachpoint_xy( location_number_ ),
                                          attachpoint_xy( other.location_number_ ) );
    }

    Point2d AttachPoint::get_2d_hex_location() const noexcept
    {
        const auto xy = attachpoint_xy( location_number_ );

        Point2d p(
            BLUE_ATTACHPOINT_SCALE * static_cast<float>( xy.first ),
            BLUE_ATTACHPOINT_SCALE * static_cast<float>( xy.second ) );

        p.xy[0] -= ( BLUE_ATTACHPOINT_SCALE * static_cast<float>( BLUE_MID_COORD ) );
        p.xy[1] -= ( BLUE_ATTACHPOINT_SCALE * static_cast<float>( BLUE_MID_COORD ) );

        // Rotate by 180 as our grid is facing the wrong way:
        p.xy[0] = -p.xy[0];
        p.xy[1] = -p.xy[1];

        return p;
    }

    Point3d AttachPoint::get_3d_hex_location() const noexcept
    {
        const auto xy = attachpoint_xy( location_number_ );
        constexpr auto board_y = 0.0f;

        Point3d pt( ( BLUE_ATTACHPOINT_SCALE ) * static_cast<float>( xy.first ),
                      board_y,
                    ( BLUE_ATTACHPOINT_SCALE ) * static_cast<float>( xy.second ) );

        pt.xyz[0] -= ( ( BLUE_ATTACHPOINT_SCALE * static_cast<float>( BLUE_MID_COORD ) ) );
        pt.xyz[2] -= ( ( BLUE_ATTACHPOINT_SCALE * static_cast<float>( BLUE_MID_COORD ) ) );

        // Rotate by 180 as our grid is facing the wrong way:
        pt.xyz[0] = -pt.xyz[0];
        pt.xyz[2] = -pt.xyz[2];

        return pt;
    }

    bool AttachPoint::operator < (const AttachPoint& rhs ) const noexcept
    {
        return location_number() < rhs.location_number();
    }

    bool AttachPoint::operator == (const AttachPoint& rhs ) const noexcept
    {
        return location_number_ == rhs.location_number_;
    }

    bool AttachPoint::operator != (const AttachPoint& rhs ) const noexcept
    {
        return location_number_ != rhs.location_number_;
    }
}
