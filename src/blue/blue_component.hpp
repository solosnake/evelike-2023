#ifndef blue_component_hpp
#define blue_component_hpp

#include <cstdint>
#include <string>
#include <string_view>
#include "solosnake/blue/blue_bgr.hpp"
#include "solosnake/blue/blue_componentcategory.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    /// Core of Hardware components. Hardware components are modules
    /// that a ship/bot is made up of, and are visible cubic units of the
    /// ship/bot, and have a mass etc. Each must have a unique name that can
    /// be used to identify its type in the system.
    class Component
    {
    public:

        /// Construct a Component and initialise all the member data.
        /// @param name Cannot be an empty string.
        Component(const std::string_view& name,
                  std::int32_t hull_hit_points,
                  std::int32_t mass,
                  ComponentCategory categorisation,
                  Amount build_cost,
                  float build_complexity);

        std::string_view component_name() const noexcept;

        std::int32_t hull_hitpoints() const noexcept;

        std::int32_t mass() const noexcept;

        ComponentCategory categorisation() const noexcept;

        const Amount& build_cost() const noexcept;

        /// A measure of how complex or high-end the Component is. Zero
        /// means simple. Higher factors slow down the build times.
        float build_complexity() const noexcept;

        bool operator == (const Component& rhs) const noexcept;
        bool operator != (const Component& rhs) const noexcept;

    private:

        std::string         component_name_{};
        std::int32_t        hull_hit_points_{0};
        std::int32_t        mass_{0};
        ComponentCategory   categorisation_{DualUseComponent};
        Amount              build_cost_;
        float               build_complexity_{1.0f};
    };
}

#include "solosnake/blue/blue_component.inl"
#endif
