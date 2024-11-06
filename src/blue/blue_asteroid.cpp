#include "solosnake/blue/blue_asteroid.hpp"

#define BLUE_ASTEROID_DEFAULT_RADIUS (1.0f)

#ifdef BLUE_ASTEROID_DEFAULT_RADIUS
#   pragma message(__FILE__ "(6): Warning: Using incorrect asteroid radius")
#endif

namespace blue
{
    Asteroid::Asteroid( const OreTypes ore,
                        const std::uint16_t vol,
                        const Hex_coord board_xy ) noexcept
        : board_xy_( board_xy )
        , remaining_volume_( vol )
        , ore_type_( static_cast<std::uint8_t>( ore ) )
        , volume_of_one_unit_( static_cast<std::uint8_t>( blue::volume_of_n_units_of( ore, 1u ) ) )
        , on_board_( true )
    {
    }

    float Asteroid::bounding_radius() const noexcept
    {
        return BLUE_ASTEROID_DEFAULT_RADIUS;
    }

    /// Extracts @a volume of ore, or less, depleting the asteroid's volume.
    /// An amount is returned, and not a Tradables, because this is actually
    /// what is used during the mining code.
    /// If the requested volume is less than the volume of a single unit of
    /// the asteroid's ore, then nothing is extracted and the empty Tradables
    /// amount is returned. This means that a good mining laser (capable of
    /// extracting a large volume) is needed to mine the richer ores as these
    /// have a larger volume per unit.
    Amount Asteroid::try_extract_ore_volume( const std::uint16_t volume ) noexcept
    {
        if( volume >= volume_of_one_unit_ )
        {
            // Clamp extracted volume to remaining volume.
            auto clamped_volume = remaining_volume_ > volume ? volume : remaining_volume_;
            remaining_volume_ -= clamped_volume;

            // There may be some loss of asteroid here.
            unsigned int units_mined = max_units_in_n_volumes_of( ore_type(), clamped_volume );

            // units mined is guaranteed to be <= volume, so safely fits into
            // 16 bit type.
            return Amount( ore_type(), static_cast<uint16_t>( units_mined ) );
        }
        else
        {
            // Nothing mined, miner is wasting his time.
            return Amount();
        }
    }

    bool Asteroid::operator == ( const Asteroid& rhs ) const noexcept
    {
        return (board_xy_ == rhs.board_xy_) &&
               (remaining_volume_ == rhs.remaining_volume_) &&
               (ore_type_ == rhs.ore_type_) &&
               (on_board_ == rhs.on_board_);

    }

    bool Asteroid::operator != ( const Asteroid& rhs ) const noexcept
    {
        return ! (*this == rhs);
    }
}
