#include <cassert>
#include "solosnake/blue/blue_hex_path_costs.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    void Hex_path_costs::set_path_cost( TileCostVariable x, std::uint8_t c ) noexcept
    {
        assert( c <= Hex_path_costs::MaxPathCost );

        // Clamp c to range, just in case of error.
        c = c > Hex_path_costs::MaxPathCost ? Hex_path_costs::MaxPathCost : c;

        switch( x )
        {
            case CostOfUnoccupiedTile:
                costs_[0] = c;
                break;

            case CostOfTileWithBot:
                costs_[1] = c;
                break;

            case CostOfTileWithAsteroid:
                costs_[2] = c;
                break;

            default:
                break;
        }

    }
}
