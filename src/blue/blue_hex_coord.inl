#include "solosnake/blue/blue_hex_coord.hpp"
#include <cassert>

#define SS_COORD_X_FROM_AXIAL(X, Z) (X)
#define SS_COORD_Y_FROM_AXIAL(X, Z) ((Z) + ((X) - ((X) & 1)) / 2)
#define SS_AXIAL_X_FROM_COORD(Q, R) (Q)
#define SS_AXIAL_Y_FROM_COORD(Q, R) ((R) - ((Q) - ((Q) & 1)) / 2)
#define SS_COORD_INVALID_VALUE      (127)

namespace blue
{
    inline bool operator == ( const Hex_coord& lhs, const Hex_coord& rhs ) noexcept
    {
        return ( lhs.x == rhs.x ) && ( lhs.y == rhs.y );
    }

    inline bool operator != ( const Hex_coord& lhs, const Hex_coord& rhs ) noexcept
    {
        return ( lhs.x != rhs.x ) || ( lhs.y != rhs.y );
    }

    inline bool operator<( const Hex_coord& lhs, const Hex_coord& rhs ) noexcept
    {
        return ( lhs.x < rhs.x ) || ( lhs.x == rhs.x && lhs.y < rhs.y );
    }

    inline std::uint16_t Hex_coord::to_uint16( const Hex_coord c ) noexcept
    {
        return ( ( 0xFF00 & ( static_cast<std::uint16_t>( c.x ) << 8u ) )
                 | ( 0x00FF & static_cast<std::uint16_t>( c.y ) ) );
    }

    inline Hex_coord Hex_coord::from_uint16( const std::uint16_t n ) noexcept
    {
        Hex_coord c
            = { static_cast<std::int8_t>( 0x00FF & ( n >> 8 ) ), static_cast<std::int8_t>( 0x00FF & n ) };
        assert( to_uint16( c ) == n );
        return c;
    }

    inline Hex_coord Hex_coord::make_coord( std::int8_t X, std::int8_t Y ) noexcept
    {
        Hex_coord c = { X, Y };
        return c;
    }

    inline Hex_coord Hex_coord::make_invalid_coord() noexcept
    {
        Hex_coord c = { std::int8_t( SS_COORD_INVALID_VALUE ), std::int8_t( SS_COORD_INVALID_VALUE ) };
        return c;
    }

    inline bool Hex_coord::is_invalid() const noexcept
    {
        return x == std::int8_t( SS_COORD_INVALID_VALUE ) && y == std::int8_t( SS_COORD_INVALID_VALUE );
    }

    inline bool Hex_coord::is_valid() const noexcept
    {
        return ! is_invalid();
    }

    inline bool Hex_coord::axial_coords_are_valid( int X, int Z ) noexcept
    {
        return ( SS_COORD_X_FROM_AXIAL( X, Z ) < +128 ) &&
               ( SS_COORD_X_FROM_AXIAL( X, Z ) > -127 ) &&
               ( SS_COORD_Y_FROM_AXIAL( X, Z ) < +128 ) &&
               ( SS_COORD_Y_FROM_AXIAL( X, Z ) > -127 );
    }

    inline Hex_coord Hex_coord::from_axial( int X, int Z ) noexcept
    {
        assert( axial_coords_are_valid( X, Z ) );

        Hex_coord c =
        {
            static_cast<std::int8_t>( SS_COORD_X_FROM_AXIAL( X, Z ) ),
            static_cast<std::int8_t>( SS_COORD_Y_FROM_AXIAL( X, Z ) )
        };

        return c;
    }

    inline Hex_coord Hex_coord::try_make_coord_from_axial( int X, int Z ) noexcept
    {
        return axial_coords_are_valid( X, Z ) ? from_axial( X, Z ) : make_invalid_coord();
    }
}
