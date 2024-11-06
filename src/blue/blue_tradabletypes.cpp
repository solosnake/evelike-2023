#include "solosnake/blue/blue_tradabletypes.hpp"
#include <cassert>

namespace blue
{
    namespace
    {
        constexpr std::string_view const typeNames[] =
        {
            "Panguite", "Kamacite",  "Ataxite",    "Chondrite",
            "Metals",   "NonMetals", "SemiMetals", "Alkalis"
        };

        constexpr TradableTypesVolumes vols[] =
        {
            VolumePanguite, VolumeKamacite,  VolumeAtaxite,    VolumeChondrite,
            VolumeMetals,   VolumeNonMetals, VolumeSemiMetals, VolumeAlkalis
        };
    }

    std::string_view tradable_type_name( TradableTypes t )
    {
        assert( t < 8u );
        return typeNames[t];
    }

    unsigned int volume_of_n_units_of( TradableTypes t, unsigned int n )
    {
        return n * vols[t];
    }

    unsigned int max_units_in_n_volumes_of( TradableTypes t, unsigned int n )
    {
        return n / vols[t];
    }
}
