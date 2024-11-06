#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_refine.hpp"
#include "solosnake/blue/blue_tradables.hpp"
#include "solosnake/blue/blue_tradabletypes.hpp"
#include "solosnake/blue/blue_unreachable.hpp"

namespace blue
{
    namespace
    {
        /// How long each ore type takes to refine relative to 1.0.
        const float RefiningPeriodFactors[] =
        {
            BLUE_REFINING_PANGUITE_FACTOR,
            BLUE_REFINING_KAMACITE_FACTOR,
            BLUE_REFINING_ATAXITE_FACTOR,
            BLUE_REFINING_CHONDRITE_FACTOR
        };
    }

    /// Returns the amount of metals @a n units of @a ore refines to.
    Amount refine( OreTypes ore, std::uint16_t n ) noexcept
    {
        std::array<std::int16_t,BLUE_TRADABLE_TYPES_COUNT> refined{};

        switch( ore )
        {
            case PanguiteOre:
                refined[Metals]     = n * BLUE_REFINED_PANGUITE_METALS;
                refined[NonMetals]  = n * BLUE_REFINED_PANGUITE_NONMETALS;
                refined[SemiMetals] = n * BLUE_REFINED_PANGUITE_SEMIMETALS;
                refined[Alkalis]    = n * BLUE_REFINED_PANGUITE_ALKALIS;
                break;

            case KamaciteOre:
                refined[Metals]     = n * BLUE_REFINED_KAMACITE_METALS;
                refined[NonMetals]  = n * BLUE_REFINED_KAMACITE_NONMETALS;
                refined[SemiMetals] = n * BLUE_REFINED_KAMACITE_SEMIMETALS;
                refined[Alkalis]    = n * BLUE_REFINED_KAMACITE_ALKALIS;
                break;

            case AtaxiteOre:
                refined[Metals]     = n * BLUE_REFINED_ATAXITE_METALS;
                refined[NonMetals]  = n * BLUE_REFINED_ATAXITE_NONMETALS;
                refined[SemiMetals] = n * BLUE_REFINED_ATAXITE_SEMIMETALS;
                refined[Alkalis]    = n * BLUE_REFINED_ATAXITE_ALKALIS;
                break;

            case ChondriteOre:
                refined[Metals]     = n * BLUE_REFINED_CHONDRITE_METALS;
                refined[NonMetals]  = n * BLUE_REFINED_CHONDRITE_NONMETALS;
                refined[SemiMetals] = n * BLUE_REFINED_CHONDRITE_SEMIMETALS;
                refined[Alkalis]    = n * BLUE_REFINED_CHONDRITE_ALKALIS;
                break;

            default:
                ss_unreachable;
                break;
        }

        return Amount( refined );
    }

    float refine_period_factor( OreTypes ore ) noexcept
    {
        return RefiningPeriodFactors[ore];
    }
}
