#ifndef blue_hex_coord_hpp
#define blue_hex_coord_hpp

#include <cstdint>

namespace blue
{
    /// This struct must not have a ctor as it is used in unions.
    struct Hex_coord
    {
        std::int8_t x;
        std::int8_t y;

        bool is_invalid() const noexcept;

        bool is_valid() const noexcept;

        static std::uint16_t  to_uint16( const Hex_coord ) noexcept;

        static Hex_coord      from_uint16( const std::uint16_t ) noexcept;

        /// Note: does not check that X and Y will create a valid Hex_coord.
        static Hex_coord      from_axial( int X, int Z ) noexcept;

        /// Returns an invalid Hex_coord if X or Z cannot form a Hex_coord.
        static Hex_coord      try_make_coord_from_axial( int X, int Z ) noexcept;

        static Hex_coord      make_coord( std::int8_t X, std::int8_t Y ) noexcept;

        static Hex_coord      make_invalid_coord() noexcept;

        static bool           axial_coords_are_valid( int X, int Z ) noexcept;
    };
}

#include "solosnake/blue/blue_hex_coord.inl"
#endif
