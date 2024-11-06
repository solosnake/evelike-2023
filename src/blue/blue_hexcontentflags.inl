#include "solosnake/blue/blue_hexcontentflags.hpp"

namespace blue
{
    inline constexpr std::uint8_t sun_strength( HexContentFlags f ) noexcept
    {
        return ( f & HEX_FLAGS_SUN_STRENGTH ) >> 4u;
    }

    inline constexpr HexContentFlags sun_strength_as_flags( std::uint8_t s ) noexcept
    {
        assert( s <= 7u );
        return static_cast<HexContentFlags>( s << 4u );
    }

    /// This tile is not part of the board.
    consteval HexContentFlags off_board_tile_value() noexcept
    {
        return HEX_FLAGS_OFF_BOARD;
    }

    inline consteval HexContentFlags sun_strength_mask() noexcept
    {
        return HEX_FLAGS_SUN_STRENGTH;
    }

    inline consteval HexContentFlags asteroid_tile_value() noexcept
    {
        return HEX_FLAGS_ASTEROID;
    }

    inline consteval HexContentFlags bot_tile_value() noexcept
    {
        return HEX_FLAGS_BOT;
    }

    inline consteval HexContentFlags empty_tile_value() noexcept
    {
        return 0u;
    }

    inline consteval HexContentFlags reserved_tile_value() noexcept
    {
        return HEX_FLAGS_RESERVED;
    }

    inline consteval HexContentFlags tile_value_mask() noexcept
    {
        return HEX_FLAGS_BOT | HEX_FLAGS_ASTEROID | HEX_FLAGS_SUN | HEX_FLAGS_RESERVED | HEX_FLAGS_OFF_BOARD;
    }

    inline constexpr bool is_not_off_board_tile( HexContentFlags f ) noexcept
    {
        return ( f & HEX_FLAGS_OFF_BOARD ) == 0u;
    }

    inline constexpr bool is_sun_or_off_board_tile( HexContentFlags f ) noexcept
    {
        return ( f & ( HEX_FLAGS_OFF_BOARD | HEX_FLAGS_SUN ) ) != 0u;
    }

    inline constexpr bool is_off_board_tile( HexContentFlags f ) noexcept
    {
        return ( f & ( HEX_FLAGS_OFF_BOARD ) ) != 0u;
    }

    inline constexpr bool is_sun_tile( HexContentFlags f ) noexcept
    {
        return ( f & ( HEX_FLAGS_SUN ) ) != 0u;
    }

    inline constexpr bool is_bot_on_tile( HexContentFlags f ) noexcept
    {
        return ( f & HEX_FLAGS_BOT ) != 0u;
    }

    inline constexpr bool is_asteroid_on_tile( HexContentFlags f ) noexcept
    {
        return ( f & HEX_FLAGS_ASTEROID ) != 0u;
    }

    inline constexpr bool is_empty_tile( HexContentFlags f ) noexcept
    {
        return ( f & ( HEX_FLAGS_BOT | HEX_FLAGS_ASTEROID | HEX_FLAGS_SUN | HEX_FLAGS_OFF_BOARD ) ) == 0u;
    }

    inline constexpr bool is_empty_and_unreserved_tile( HexContentFlags f ) noexcept
    {
        return ( f & tile_value_mask() ) == 0u;
    }

    inline constexpr bool is_reserved_tile( HexContentFlags f ) noexcept
    {
        return ( f & HEX_FLAGS_RESERVED ) != 0u;
    }
}