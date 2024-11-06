#include "solosnake/blue/blue_hardpointtype.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    std::string_view to_string(HardpointType h)
    {
        switch(h)
        {
            case WeaponHardpoint:
                return std::string_view("WeaponHardpoint");
            case CapDrainHardpoint:
                return std::string_view("CapDrainHardpoint");
            case CapXFerHardpoint:
                return std::string_view("CapXFerHardpoint");
            case ArmourRepairerHardpoint:
                return std::string_view("ArmourRepairerHardpoint");
            case MinerHardpoint:
                return std::string_view("MinerHardpoint");
            default:
                ss_throw("Invalid HardpointType value.");
        };
    }

    HardpointType hardpoint_type_from_string(const std::string_view& text)
    {
        if(text == "WeaponHardpoint")
        {
            return WeaponHardpoint;
        }

        if(text == "CapDrainHardpoint")
        {
            return CapDrainHardpoint;
        }

        if(text == "CapXFerHardpoint")
        {
            return CapXFerHardpoint;
        }

        if(text == "ArmourRepairerHardpoint")
        {
            return ArmourRepairerHardpoint;
        }

        if(text == "MinerHardpoint")
        {
            return MinerHardpoint;
        }

        ss_throw("Invalid HardpointType string.");
    }
}
