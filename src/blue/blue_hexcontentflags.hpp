#ifndef blue_hexcontentflags_hpp
#define blue_hexcontentflags_hpp

#include <cassert>
#include <cstdint>

namespace blue
{
    /// Hex contents are the flags used to store details of the state and
    /// contents of a hexagonal tile in the grid. Tiles can be invalid,
    /// reserved, and may or may not have something on them.
    using HexContentFlags = std::uint8_t;

    /// A reserved hex means it is fully or partially occupied. Two hex can be
    /// reserved at once by a user if it is transitioning across them.
    /// Sun tiles are also always off-board tiles.
    constexpr std::uint8_t HEX_FLAGS_BOT            {0x01u};  // 0000 0001
    constexpr std::uint8_t HEX_FLAGS_ASTEROID       {0x02u};  // 0000 0010
    constexpr std::uint8_t HEX_FLAGS_SUN            {0x04u};  // 0000 0100
    constexpr std::uint8_t HEX_FLAGS_RESERVED       {0x08u};  // 0000 1000
    constexpr std::uint8_t HEX_FLAGS_SUN_STRENGTH   {0x70u};  // 0111 0000
    constexpr std::uint8_t HEX_FLAGS_OFF_BOARD      {0x80u};  // 1000 0000

    /// This tile is not part of the board.
    consteval HexContentFlags off_board_tile_value() noexcept;

    consteval HexContentFlags sun_strength_mask() noexcept;

    consteval HexContentFlags asteroid_tile_value() noexcept;

    consteval HexContentFlags bot_tile_value() noexcept;

    consteval HexContentFlags empty_tile_value() noexcept;

    consteval HexContentFlags reserved_tile_value() noexcept;

    consteval HexContentFlags tile_value_mask() noexcept;

    constexpr HexContentFlags sun_strength_as_flags( std::uint8_t s ) noexcept;

    constexpr std::uint8_t sun_strength( HexContentFlags f ) noexcept;

    constexpr bool is_not_off_board_tile( HexContentFlags f ) noexcept;

    constexpr bool is_sun_or_off_board_tile( HexContentFlags f ) noexcept;

    constexpr bool is_off_board_tile( HexContentFlags f ) noexcept;

    constexpr bool is_sun_tile( HexContentFlags f ) noexcept;

    constexpr bool is_bot_on_tile( HexContentFlags f ) noexcept;

    constexpr bool is_asteroid_on_tile( HexContentFlags f ) noexcept;

    constexpr bool is_empty_tile( HexContentFlags f ) noexcept;

    constexpr bool is_empty_and_unreserved_tile( HexContentFlags f ) noexcept;

    constexpr bool is_reserved_tile( HexContentFlags f ) noexcept;
}

#include "solosnake/blue/blue_hexcontentflags.inl"
#endif
