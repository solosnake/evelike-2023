#include "solosnake/blue/blue_tradabletypes.hpp"

namespace blue
{
    inline unsigned int volume_of_n_units_of( OreTypes t, unsigned int n )
    {
        return volume_of_n_units_of( static_cast<TradableTypes>( t ), n );
    }

    inline unsigned int max_units_in_n_volumes_of( OreTypes t, unsigned int n )
    {
        return max_units_in_n_volumes_of( static_cast<TradableTypes>( t ), n );
    }

    inline constexpr bool is_ore( const std::uint8_t ore ) noexcept
    {
        return ( ore == PanguiteOre ) || ( ore == KamaciteOre )
            || ( ore == AtaxiteOre )  || ( ore == ChondriteOre );
    }
}