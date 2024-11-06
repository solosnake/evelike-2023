#include <vector>
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_unreachable.hpp"

namespace blue
{
    bool is_validate_categorisation( ComponentCategory c, HardpointType h )
    {
        bool result = false;

        switch( c )
        {
            case OffensiveComponent:
                result = (h == WeaponHardpoint || h == CapDrainHardpoint);
                break;

            case DefensiveComponent:
                result = (h == ArmourRepairerHardpoint);
                break;

            case IndustrialComponent:
                result = (h == MinerHardpoint || h == CapXFerHardpoint);
                break;

            // There are no dual-use hardpoints, it messes up Classification.
            case DualUseComponent:
            case PropulsionComponent:
            default:
                break;
        }

        return result;
    }

    Hardpoint::Hardpoint( const Component& c,
                          const Hardpoint_shooter& shooter)
        : component_(c)
        , shooter_(shooter)
    {
        if(! is_validate_categorisation(c.categorisation(), shooter.hardpoint_type()))
        {
            ss_throw("Bad hardpoint categorisation.");
        }
    }

    bool Hardpoint::operator == (const Hardpoint& rhs ) const noexcept
    {
        return component_ == rhs.component_
            && shooter_   == rhs.shooter_ ;
    }

    bool Hardpoint::operator != (const Hardpoint& rhs ) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
