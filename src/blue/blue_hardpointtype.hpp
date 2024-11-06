#ifndef blue_hardpointttype_hpp
#define blue_hardpointttype_hpp

#include <cstdint>
#include <string_view>

namespace blue
{
    /// Hardpoints cannot be classified as dual-use, as this interferes
    /// with the naming system used by the Classification.
    enum HardpointType : std::uint8_t
    {
        WeaponHardpoint,         ///< Offensive.
        CapDrainHardpoint,       ///< Offensive.
        CapXFerHardpoint,        ///< XFer is classed as industrial.
        ArmourRepairerHardpoint, ///< Defensive.
        MinerHardpoint           ///< Industrial
    };

    std::string_view to_string(HardpointType);

    HardpointType hardpoint_type_from_string(const std::string_view&);
}

#endif
