#ifndef blue_thruster_hpp
#define blue_thruster_hpp

#include <string>
#include <vector>
#include "solosnake/blue/blue_component.hpp"

namespace blue
{
    /// A special type of Softpoint with a limited set of attach points
    /// and which are required to allow a Chassis to be mobile.
    class Thruster final
    {
    public:

        Thruster(const Component&, std::int32_t engine_power);

        std::int32_t power() const noexcept;

        const Component& details() const noexcept;

        bool operator== (const Thruster& ) const noexcept;
        bool operator!= (const Thruster& ) const noexcept;

    private:

        Component    component_;
        std::int32_t engine_power_;
    };
}

#include "solosnake/blue/blue_thruster.inl"
#endif
