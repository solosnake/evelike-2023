#ifndef blue_componentcategory_hpp
#define blue_componentcategory_hpp

#include <cstdint>
#include <string_view>

namespace blue
{
    /// Components are categorized as follows:
    /// Offensive -   Hardpoints only - Weapons
    /// Defensive -   Hardpoints only - Repairers
    /// Industrial -  Hardpoints/softpoints - mining lasers, cargo holds,
    ///               cap xfer, construction.
    /// Propulsion -  Thrusters only, all thrusters.
    /// DualUse -     Can be industrial AND/OR Offensive/Defensive. All
    ///               components which are not classed as purely Offensive,
    ///               purely Defensive or purely Industrial are classed
    ///               as dual use components - electronics, sensors, CPUs etc.
    enum ComponentCategory : std::int32_t
    {
        OffensiveComponent,
        DefensiveComponent,
        IndustrialComponent,
        PropulsionComponent,
        DualUseComponent
    };

    constexpr auto BLUE_COMPONENTCATEGORY_COUNT = 5;

    /// Returns the ComponentCategory value as a string, or throws.
    std::string_view  to_string(ComponentCategory);

    /// Returns the ComponentCategory enum from the string, or throws.
    ComponentCategory component_category_from_string(const std::string_view&);
}

#endif