#ifndef blue_asteroid_hpp
#define blue_asteroid_hpp

#include <cstdint>
#include <string>
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    /// Asteroids are volumes of a single ore occupying a single tile.
    /// They are 'in grid' by default, assuming the board will add them,
    /// and can be removed once from the board.
    class Asteroid
    {
    public:

        Asteroid(
            const OreTypes ore,
            const std::uint16_t vol,
            const Hex_coord board_xy ) noexcept;

        /// Total remaining volume in game volume units. When zero the
        /// Asteroid is gone.
        std::uint16_t volume() const noexcept;

        /// Panguite, Kamacite, Ataxite or Chondrite.
        OreTypes ore_type() const noexcept;

        Hex_coord board_xy() const noexcept;

        /// Bounding radius in world units.
        float bounding_radius() const noexcept;

        Amount try_extract_ore_volume( const std::uint16_t volume ) noexcept;

        /// Initially true, tracks whether the board considers this Asteroid to
        /// be in the grid (it will remove all asteroids without volume once
        /// per frame).
        bool is_on_board() const noexcept;

        /// Stores the fact that this Asteroid no longer appears on the board.
        void set_as_removed_from_board() noexcept;

        bool operator == ( const Asteroid& rhs ) const noexcept;
        bool operator != ( const Asteroid& rhs ) const noexcept;

    private:
        Hex_coord       board_xy_;
        std::uint16_t   remaining_volume_;
        std::uint8_t    ore_type_;
        std::uint8_t    volume_of_one_unit_;
        bool            on_board_;
    };
}

#include "solosnake/blue/blue_asteroid.inl"

#endif
