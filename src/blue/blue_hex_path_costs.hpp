#ifndef blue_hex_path_costs_hpp
#define blue_hex_path_costs_hpp

#include "solosnake/blue/blue_unreachable.hpp"
#include "solosnake/blue/blue_hexsenseresult.hpp"

namespace blue
{
    /// Allows a Machine to customise the costs of moving around on the board
    /// when calculating navigation paths with A*. This object only stores the
    /// cost variables: it does not perform the calculations.
    class Hex_path_costs
    {
    public:

        /// CostOfInvalidTile is not actually allowed vary.
        enum TileCostVariable : std::uint16_t
        {
            CostOfUnoccupiedTile    = 0,
            CostOfTileWithBot       = 1,
            CostOfTileWithAsteroid  = 2,
            CostOfInvalidTile       = 3
        };

        static constexpr std::uint8_t MaxPathCost = 9u;

        /// Default costs mean no movement through occupied tiles, all other
        /// tiles are equally rated.
        Hex_path_costs() noexcept;

        /// Costs returned are in the range 0 - 9. Zero is special and
        /// indicates that a tile with this content is not traversable.
        std::uint8_t get_path_cost( HexContentFlags x ) const noexcept;

        /// Sets the cost of traversing a tile occupied by x. If c is zero then
        /// the tile will not be considered as traversable by the A* algorithm.
        /// The cost of HexIsVoid cannot be changed - it is always zero.
        /// Attempting to set it will be ignored.
        /// @pre c must be between 0 and MaxPathCost. 
        void set_path_cost( TileCostVariable x, std::uint8_t c ) noexcept;

    private:

        std::uint8_t costs_[4]; // unoccupied, bot, Asteroid, offboard.
    };
}

#include "solosnake/blue/blue_hex_path_costs.inl"
#endif
