#ifndef blue_combat_system_hpp
#define blue_combat_system_hpp

#include <cassert>
#include <algorithm>

namespace blue
{
    class Randoms;

    /// A block of damage dealt, broken down into the type of damages.
    /// @note This struct must not have a ctor as it appears in a union.
    struct Damages
    {
        float kinetic_dmg() const noexcept;

        float thermal_dmg() const noexcept;

        float em_dmg() const noexcept;

        float radiation_dmg() const noexcept;

        Damages  operator*( float f ) const noexcept;

        Damages& operator*=( float f ) noexcept;

        bool operator == (const Damages& rhs) const noexcept;

        float dmg_type[4];
    };

    /// Resistances are more factors applied to damage. Lower is good,
    /// a resistance of 0 means zero damage gets through.
    struct Resistances
    {
        Resistances() = default;

        Resistances( float kin, float therm, float em, float rad );

        float kinetic_resist() const noexcept;

        float thermal_resist() const noexcept;

        float em_resist() const noexcept;

        float radiation_resist() const noexcept;

        float resistance[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

    /// We dont permit reparation of hull.
    class Combat_system
    {
    public:

        enum DamageTypes
        {
            Kinetic,
            Thermal,
            ElectroMagnetic,
            Radiation
        };

        Combat_system() = default;

        Combat_system( unsigned int totalHull, unsigned int totalArmour, const Resistances& );

        void apply_damage( const Damages& d ) noexcept;

        void repair_armour( int h ) noexcept;

        /// Returns true if the hull health is greater than zero.
        bool is_alive() const noexcept;

        /// Sets hull to zero, effectively "killing" this object.
        void kill() noexcept;

        /// Returns true if the health is still positive after all the
        /// previous turn's attacks and heals are completed.
        bool resolve( Randoms& randoms );

    private:

        void resolve_combat( Randoms& randoms );

    private:

        int             accumulated_damage_             = 0;
        int             accumulated_armour_repair_      = 0;
        int             armour_                         = 0;
        int             hull_                           = 0;
        int             total_armour_                   = 0;
        Resistances     resistance_;
    };
}

#include "solosnake/blue/blue_combat_system.inl"
#endif
