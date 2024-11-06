#ifndef blue_fixed_angle_hpp
#define blue_fixed_angle_hpp

#include <cstdint>
#include <cassert>
#include "solosnake/blue/blue_hexfacingname.hpp"
#include "solosnake/blue/blue_radians.hpp"
#include "solosnake/blue/blue_throw.hpp"

// This number is chosen to be easily divisible by 6 and 4, so we get         //
// perfect alignment with the 6 faces.                                        //
//                                                                            //
//    /\ /\                                                                   //
//   |0 |1 |                                                                  //
//  / \/ \/ \                                                                 //
//  |5 |  |2 |                                                                //
//  \ /\ /\ /                                                                 //
//   |4 |3 |                                                                  //
//    \/ \/                                                                   //
//                                                                            //
// 3,600,000 is 0x36EE80 in hex.                                              //
// In an int32 the max value is ~2,147,483,647.                               //
// 2,147,483,647 / 3,600,000 = 596 - this is thus the max number of whole     //
// revolutions we can possibly agree to do before overflow.                   //
// 498 = 83 * 6. If we modulo a large number by 498 we still end up at the    //
// desired facing.                                                            //


constexpr float BLUE_PI             {3.1415926535897932384626433832795f};
constexpr float BLUE_TWO_PI         {6.283185307179586476925286766559f};

/// How many units 1/6 PI is divided into. The game works in integer units
/// not floating points. We divided the unit circle into discrete units.
/// One degree is 10000 units. A full circle is 3'600'000 units.

constexpr std::int32_t BLUE_INT32_SIXTH_PI          {300000};
constexpr std::int32_t BLUE_INT32_TWO_PI            {(BLUE_INT32_SIXTH_PI)*12};
constexpr std::int32_t BLUE_INT32_THREE_QUARTERS_PI {(BLUE_INT32_SIXTH_PI)*9};
constexpr std::int32_t BLUE_INT32_PI                {(BLUE_INT32_SIXTH_PI)*6};
constexpr std::int32_t BLUE_INT32_HALF_PI           {(BLUE_INT32_SIXTH_PI)*3};
constexpr std::int32_t BLUE_INT32_THIRD_PI          {(BLUE_INT32_SIXTH_PI)*2};

constexpr float BLUE_FLOAT32_FIXED_TWO_PI           {float{BLUE_INT32_TWO_PI}};

constexpr std::int32_t BLUE_FACING_0                {(BLUE_INT32_SIXTH_PI)*9};
constexpr std::int32_t BLUE_FACING_1                {(BLUE_INT32_SIXTH_PI)*11};
constexpr std::int32_t BLUE_FACING_2                {BLUE_INT32_SIXTH_PI};
constexpr std::int32_t BLUE_FACING_3                {(BLUE_INT32_SIXTH_PI)*3};
constexpr std::int32_t BLUE_FACING_4                {(BLUE_INT32_SIXTH_PI)*5};
constexpr std::int32_t BLUE_FACING_5                {(BLUE_INT32_SIXTH_PI)*7};

namespace blue
{
    using Fixed_angle = std::int32_t;

    inline constexpr bool is_facing_angle( Fixed_angle a ) noexcept
    {
        return (a == BLUE_FACING_0) || (a == BLUE_FACING_1) || (a == BLUE_FACING_2)
            || (a == BLUE_FACING_3) || (a == BLUE_FACING_4) || (a == BLUE_FACING_5);
    }

    inline constexpr std::int32_t degrees_to_fixed_angle( std::int16_t d ) noexcept
    {
        return Fixed_angle{d * 10000};
    }

    inline std::uint16_t facing_name_to_uint16( HexFacingName facing ) noexcept
    {
        return static_cast<std::uint16_t>( facing );
    }

    /// Checks that @a f is a valid facing name, then casts. @a f is NOT
    /// an angle, it is a FacingName as an unsigned int!
    inline HexFacingName cast_to_facing_name( const std::uint16_t f ) noexcept
    {
        assert( f < 6 );
        return static_cast<HexFacingName>( f );
    }

    inline Fixed_angle fixedAngle_from_facing( HexFacingName facing ) noexcept
    {
        const Fixed_angle angles[] =
        {
            BLUE_FACING_0, BLUE_FACING_1, BLUE_FACING_2,
            BLUE_FACING_3, BLUE_FACING_4, BLUE_FACING_5
        };

        return angles[facing % 6];
    }

    inline HexFacingName facing_from_fixedAngle( Fixed_angle angle )
    {
        assert( angle >= 0 && angle < BLUE_INT32_TWO_PI );
        assert( blue::is_facing_angle( angle ) );

        if( BLUE_FACING_0 == angle )
        {
            return FacingTile0;
        }

        if( BLUE_FACING_1 == angle )
        {
            return FacingTile1;
        }

        if( BLUE_FACING_2 == angle )
        {
            return FacingTile2;
        }

        if( BLUE_FACING_3 == angle )
        {
            return FacingTile3;
        }

        if( BLUE_FACING_4 == angle )
        {
            return FacingTile4;
        }

        if( BLUE_FACING_5 == angle )
        {
            return FacingTile5;
        }

        ss_throw( "Angle was not a facing angle!" );
    }

    inline Radians fixed_angle_to_radians( const Fixed_angle a )
    {
        assert( a >= 0 && a < BLUE_INT32_TWO_PI );
        return Radians(
                   ( ( a + BLUE_INT32_TWO_PI ) % BLUE_INT32_TWO_PI ) * BLUE_TWO_PI / BLUE_FLOAT32_FIXED_TWO_PI );
    }
}

#endif
