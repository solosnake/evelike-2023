#ifndef blue_classification_hpp
#define blue_classification_hpp

#include <cstdint>
#include "solosnake/blue/blue_componentcategory.hpp"

namespace blue
{
    class Blueprint;

    /// Type representing a Classification system for machines.
    /// Returns information about the components used to construct a Machine as
    /// a single string. Each Machine has 1 to 23 components. Zero components
    /// is obviously the null Machine, but the Classification can represent
    /// this. Non machines (e.g. asteroids) can have this Classification. This
    /// is also the default Classification. An Asteroid has the Classification
    /// "A-AAAA". 'A' means zero components. The leading letter is the total
    /// number of components and so all A class objects have suffix AAAA.
    /// Machines have prefixes B (meaning 1 Component) to 'X' (23 components).
    class Classification
    {
    public:

        /// Character between Component count and Component breakdown in string.
        static constexpr char CountSeparator = '-';

        /// Default Classification is "A-AAAA" (dual use A).
        Classification();

        /// Analyses the blueprint to produces its Classification.
        explicit Classification( const Blueprint& );

        /// Returns the classification as a NUL terminated string.
        const char* c_str() const noexcept;

        /// Returns the number of components in this category in a Machine with
        /// this Classification.
        std::uint16_t component_count( ComponentCategory c ) const noexcept;

        /// How many components a Machine of this Classification has.
        std::uint16_t total_components_count() const noexcept;

    private:

        /// Ensure that the size of this array is in synch with the Component
        /// categories enumerations.
        /// Each char means: Number, -, Offensive, Defensive, Industrial,
        /// Propulsion, NULL, DualUse
        char classification_[8];
    };
}

#include "solosnake/blue/blue_classification.inl"
#endif
