#ifndef blue_chassis_hpp
#define blue_chassis_hpp

#include <string>
#include <string_view>
#include <vector>
#include "solosnake/blue/blue_emissive_channels.hpp"

namespace blue
{
    /// A special type of Softpoint with a limited set of attach points
    /// and which are required to allow a Chassis to be mobile.
    class Chassis
    {
    public:

        // TODO DAIRE CHASSIS NEEDS LIST OF POSSIBLE HARD/SOFT/THRUSTER SLOTS.

        Chassis();

        Chassis(const std::string_view& name,
                const Emissive_channels& channels);

        const std::string& name() const;

        const Emissive_channels& channels() const;

        bool operator == (const Chassis& rhs) const noexcept;
        bool operator != (const Chassis& rhs) const noexcept;

    private:
        std::string          chassis_name_;
        Emissive_channels    channels_;
    };
}

#endif
