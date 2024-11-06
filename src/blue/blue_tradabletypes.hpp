#ifndef blue_tradabletypes_hpp
#define blue_tradabletypes_hpp

#include <cstdint>
#include <string_view>

namespace blue
{
    /// The types of tradable units in blue. Ordered generally from low worth
    /// to highest worth.
    ///
    /// To be able to hold these in cargo holds, the max number of cargo
    /// containers would be 23 (max components). Max uint16 is 65536. We must
    /// be able to return the volume of a complete cargo hold, and also its
    /// contents.
    ///
    /// So the max number of items in 1 hold can be 65536 / 23, and this is
    /// also the max possible volume ~= 2849 (0xB21)
    ///
    /// Ore takes up more space than minerals.
    /// We need to be able to hold 23 containers each full of the smallest
    /// possible mineral.
    /// If we set the max container volume to be 1,000, then it should be
    /// impossible to exceed 65,536.
    enum TradableTypes : std::uint8_t
    {
        /// http://en.wikipedia.org/wiki/Panguite titanium, scandium, aluminium,
        /// magnesium, zirconium, calcium, and oxygen
        Panguite,

        /// http://en.wikipedia.org/wiki/Kamacite  iron and nickel, usually in
        /// the proportions of 90:10 to 95:5 although impurities such as cobalt
        /// or carbon may be present.
        Kamacite,

        /// http://en.wikipedia.org/wiki/Ataxite iron meteorites with a high
        /// nickel content
        Ataxite,

        /// http://en.wikipedia.org/wiki/Chondrite#Composition silicates (Si04),
        /// metal, and sulfide (C-S-C), magnesium
        Chondrite,

        /// Iron, Nickel, Titanium, Scandium, Aluminium
        Metals,

        /// Carbon, Nitrogen, Phosporos, Sulfer, Oxygen
        NonMetals,

        /// Boron, Silicon,
        SemiMetals,

        /// Lithium, Sodium, Calcium, Magnesium etc
        Alkalis
    };

    /// Same values as the types, but restricted to only the refinable ores.
    enum OreTypes : std::uint8_t
    {
        PanguiteOre  = Panguite,
        KamaciteOre  = Kamacite,
        AtaxiteOre   = Ataxite,
        ChondriteOre = Chondrite
    };

    /// A single unit of each type requires the following volume of cargo
    /// space. At least one of these units must have volume 1. Richer ores
    /// should have a larger volume per unit, requiring better lasers to mine.
    /// No tradable type should have a volume greater than 255.
    enum TradableTypesVolumes : std::uint8_t
    {
        VolumePanguite      = 10,
        VolumeKamacite      = 10,
        VolumeAtaxite       = 10,
        VolumeChondrite     = 10,
        VolumeMetals        = 1,
        VolumeNonMetals     = 1,
        VolumeSemiMetals    = 1,
        VolumeAlkalis       = 1
    };

    // Constants:
    // ----------

    constexpr auto BLUE_TRADABLE_TYPES_COUNT = 8u;
    constexpr auto BLUE_REFINABLES_BEGIN     = Panguite;
    constexpr auto BLUE_REFINABLES_END       = Metals;
    constexpr auto BLUE_MAX_CONTAINER_VOLUME = 1000;

    /// Returns the name of the tradable.
    std::string_view tradable_type_name( TradableTypes );

    unsigned int volume_of_n_units_of( TradableTypes t, unsigned int n );

    unsigned int max_units_in_n_volumes_of( TradableTypes t, unsigned int n );

    unsigned int volume_of_n_units_of( OreTypes t, unsigned int n );

    unsigned int max_units_in_n_volumes_of( OreTypes t, unsigned int n );

    unsigned int volume_of_n_units_of( OreTypes t, unsigned int n );

    /// Returns how many units of e.g. Panguite are in 'n' unit volumes. As a
    /// unit of any type is always at least one unit volume, then the value
    /// returned is guaranteed to be less than or equal to n. 10 units cubed
    /// might produce 10 or 1 unit of 't'.
    unsigned int max_units_in_n_volumes_of( OreTypes t, unsigned int n );

    /// Returns true if the value if @a ore is the same as one of the OreTypes enums or
    /// one of the TradableTypes ore enums.
    constexpr bool is_ore( const std::uint8_t ore ) noexcept;
}

#include "solosnake/blue/blue_tradabletypes.inl"
#endif
