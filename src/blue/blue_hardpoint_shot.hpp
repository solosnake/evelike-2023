#ifndef blue_hardpointshot_hpp
#define blue_hardpointshot_hpp

#include <cstdint>
#include "solosnake/blue/blue_combat_system.hpp"
#include "solosnake/blue/blue_hardpointtype.hpp"

namespace blue
{
    struct Weapon_shot
    {
        float kinetic_dmg() const noexcept   { return damage.kinetic_dmg(); }
        float thermal_dmg() const noexcept   { return damage.thermal_dmg(); }
        float em_dmg() const noexcept        { return damage.em_dmg(); }
        float radiation_dmg() const noexcept { return damage.radiation_dmg(); }

        Damages damage;
    };

    struct Capdrain_shot
    {
        std::uint16_t   cap_drained;
    };

    struct Capxfer_shot
    {
        std::uint16_t   cap_transferred;
    };

    struct Repairer_shot
    {
        std::uint16_t   armour_repaired;
    };

    struct Miner_shot
    {
        std::uint16_t   volume_mined;
    };

    union Shot_effects
    {
        Weapon_shot     weapon;
        Capdrain_shot   cap_drain;
        Capxfer_shot    cap_xfer;
        Repairer_shot   repairs;
        Miner_shot      mining;
    };

    /// Details of the effect etc of a Hardpoint shot.
    /// Note: Hardpoint_shooter class relies on Capdrain_shot, Capxfer_shot etc
    /// all being the same layout (std::uint16_t).
    struct Hardpoint_shot
    {
        HardpointType   hardpoint_type_;    // Which type of Hardpoint generated this shot.
        std::uint8_t    hardpoint_index_;   // Which Hardpoint of the bot created this shot.
        std::uint16_t   max_range_;         // How far the shot could travel.
        Shot_effects    shot_effect_;       // Union: the type is indicated by `hardpoint_type_`.
    };

    bool operator == (const Hardpoint_shot& lhs, const Hardpoint_shot& rhs) noexcept;
    bool operator != (const Hardpoint_shot& lhs, const Hardpoint_shot& rhs) noexcept;

    /// Returns true if both Shot_effects compare equal.
    bool equal( HardpointType, const Shot_effects& lhs, const Shot_effects& rhs ) noexcept;
}

#endif
