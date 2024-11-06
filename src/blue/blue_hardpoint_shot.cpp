#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hardpointtype.hpp"

namespace blue
{
    bool equal( HardpointType hpt, const Shot_effects& lhs, const Shot_effects& rhs ) noexcept
    {
        bool result = false;

        switch(hpt)
        {
            case WeaponHardpoint:
                result = lhs.weapon.damage == rhs.weapon.damage;
                break;

            case CapDrainHardpoint:
                result = lhs.cap_drain.cap_drained == rhs.cap_drain.cap_drained;
                break;

            case CapXFerHardpoint:
                result = lhs.cap_xfer.cap_transferred == rhs.cap_xfer.cap_transferred;
                break;

            case ArmourRepairerHardpoint:
                result = lhs.repairs.armour_repaired == rhs.repairs.armour_repaired;
                break;

            case MinerHardpoint:
                result = lhs.mining.volume_mined == rhs.mining.volume_mined;
                break;

            default:
                break;
        }

        return result;
    }

    bool operator == (const Hardpoint_shot& lhs, const Hardpoint_shot& rhs) noexcept
    {
        return (lhs.hardpoint_index_ == rhs.hardpoint_index_)
            && (lhs.hardpoint_type_  == rhs.hardpoint_type_)
            && (lhs.max_range_       == rhs.max_range_)
            && equal(lhs.hardpoint_type_, lhs.shot_effect_, rhs.shot_effect_)
            ;
    }

    bool operator != (const Hardpoint_shot& lhs, const Hardpoint_shot& rhs) noexcept
    {
        return ! (lhs == rhs);
    }
}