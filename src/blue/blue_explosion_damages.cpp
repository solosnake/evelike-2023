#include "solosnake/blue/blue_explosion_damages.hpp"
#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_machine_events.hpp"

namespace blue
{
    Damages get_explosion_damages( const Machine_exploded& boom )
    {
        const float halfTotalDmg    = 0.50f * boom.explosionDamage;
        const float nuclear         = 0.01f * boom.explosionPercentNuclear;
        const float conventional    = 1.00f - nuclear;
        const float conventionalDmg = halfTotalDmg * conventional;
        const float nuclearDmg      = halfTotalDmg * nuclear;

        Damages dmg;
        dmg.dmg_type[Combat_system::Thermal]         = 0.125f * nuclearDmg + 0.5f * conventionalDmg;
        dmg.dmg_type[Combat_system::Kinetic]         = 0.125f * nuclearDmg + 0.5f * conventionalDmg;
        dmg.dmg_type[Combat_system::ElectroMagnetic] = 0.375f * nuclearDmg;
        dmg.dmg_type[Combat_system::Radiation]       = 0.375f * nuclearDmg;

        return dmg;
    }

    Machine_exploded get_machines_explosive_power( const Machine& m ) noexcept
    {
        const float dmg = m.capacitance_remaining() * BLUE_EXPLOSIVE_DMG_PER_CAPACITANCE
                          + m.components_count() * BLUE_EXPLOSIVE_DMG_PER_COMPONENT;

        Machine_exploded ex;

        // Clamp damage to upper limit of unsigned int.
        ex.explosionDamage = std::min<std::uint16_t>( 0xFFFF, static_cast<std::uint16_t>( dmg ) );

        ex.explosionPercentNuclear = static_cast<std::uint16_t>(
                                         std::floor( 100.0f * m.capacitance_proportion_from_nuclear_power() ) );

        // Minimum range is 1 : this allows all machines to be built and used
        // as "missiles"
        ex.explosionRange = static_cast<std::uint16_t>(
                                std::max( 1.0f, BLUE_EXPLOSIVE_RANGE_PER_COMPONENT_F * m.components_count() ) );

        ex.gridXY_ = m.board_xy();

        return ex;
    }
}
