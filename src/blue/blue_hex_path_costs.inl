#include "solosnake/blue/blue_hex_path_costs.hpp"

namespace blue
{
    inline Hex_path_costs::Hex_path_costs() noexcept
    {
        costs_[HexIsUnoccupied] = 1u;

        // 0 means impassable: by default a bot cannot navigate through
        // tiles occupied by asteroids or other bots. Zero means that this
        // is not navigable.
        costs_[HexHasBot]       = 0u;
        costs_[HexHasAsteroid]  = 0u;
        costs_[HexIsOffBoard]   = 0u;
    }

    inline std::uint8_t Hex_path_costs::get_path_cost( HexContentFlags x ) const noexcept
    {
        if( is_empty_and_unreserved_tile( x ) )
        {
            return costs_[HexIsUnoccupied];
        }
        else if( is_bot_on_tile( x ) )
        {
            return costs_[HexHasBot];
        }
        else if( is_asteroid_on_tile( x ) )
        {
            return costs_[HexHasAsteroid];
        }

        // Returning zero means that this is not navigable.
        return std::uint8_t{0};
    }
}
