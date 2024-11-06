#include <cassert>
#include "solosnake/blue/blue_classification.hpp"
#include "solosnake/blue/blue_blueprint.hpp"

namespace blue
{
    Classification::Classification( const Blueprint& bp )
    {
        assert( bp.component_count() <= Blueprint::MaxComponents );

        char offensiveHardpoints = 0;
        char defensiveHardpoints = 0;
        char industrialHardpoints = 0;
        for( size_t i = 0; i < bp.hardpoint_count(); ++i )
        {
            ComponentCategory c = bp.hardpoint_at( i ).details().categorisation();
            switch( c )
            {
                case OffensiveComponent:
                    ++offensiveHardpoints;
                    break;
                case DefensiveComponent:
                    ++defensiveHardpoints;
                    break;
                case IndustrialComponent:
                    ++industrialHardpoints;
                    break;
                case DualUseComponent:
                case PropulsionComponent:
                default:
                    ss_throw( "Hardpoint which was not offensive/defensive/industrial." );
            }
        }

        char offensiveSoftpoints = 0;
        char defensiveSoftpoints = 0;
        char industrialSoftpoints = 0;
        char dualUseSoftpoint = 0;
        for( size_t i = 0; i < bp.softpoint_count(); ++i )
        {
            ComponentCategory c = bp.softpoint_at( i ).details().categorisation();
            switch( c )
            {
                case OffensiveComponent:
                    ++offensiveSoftpoints;
                    break;
                case DefensiveComponent:
                    ++defensiveSoftpoints;
                    break;
                case IndustrialComponent:
                    ++industrialSoftpoints;
                    break;
                case DualUseComponent:
                    ++dualUseSoftpoint;
                    break;
                case PropulsionComponent:
                default:
                    ss_throw( "Softpoint which was not "
                              "offensive/defensive/industrial/dual-use." );
            }
        }

        assert( bp.component_count() == static_cast<unsigned int>( ( dualUseSoftpoint + static_cast<char>( bp.thrusters_count() )
                                          + ( offensiveHardpoints + offensiveSoftpoints )
                                          + ( defensiveHardpoints + defensiveSoftpoints )
                                          + ( industrialHardpoints + industrialSoftpoints ) ) ) );

        classification_[0] = 'A' + static_cast<char>( bp.component_count() );
        classification_[1] = Classification::CountSeparator;
        classification_[2] = 'A' + ( offensiveHardpoints + offensiveSoftpoints );
        classification_[3] = 'A' + ( defensiveHardpoints + defensiveSoftpoints );
        classification_[4] = 'A' + ( industrialHardpoints + industrialSoftpoints );
        classification_[5] = 'A' + static_cast<char>( bp.thrusters_count() );
        classification_[6] = 0; // nul
        classification_[7] = 'A' + dualUseSoftpoint;
    }
}
