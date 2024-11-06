#include "solosnake/blue/blue_combat_system.hpp"
#include "solosnake/blue/blue_randoms.hpp"

namespace blue
{
    Resistances::Resistances( float kin, float therm, float em, float rad )
    {
        assert( kin >= 0.0f && kin <= 1.0f );
        assert( therm >= 0.0f && therm <= 1.0f );
        assert( em >= 0.0f && em <= 1.0f );
        assert( rad >= 0.0f && rad <= 1.0f );
        resistance[Combat_system::Kinetic]         = 1.0f - kin;
        resistance[Combat_system::Thermal]         = 1.0f - therm;
        resistance[Combat_system::ElectroMagnetic] = 1.0f - em;
        resistance[Combat_system::Radiation]       = 1.0f - rad;
    }

    Combat_system::Combat_system( unsigned int totalHull, unsigned int totalArmour, const Resistances& r )
        : accumulated_damage_( 0 )
        , accumulated_armour_repair_( 0 )
        , armour_( totalArmour )
        , hull_( totalHull )
        , total_armour_( totalArmour )
        , resistance_( r )
    {
    }

    void Combat_system::resolve_combat( Randoms& randoms )
    {
        // We attempt to simulate a sequence of damages and repairs across
        // a unit of time. These may have arrived in different orders, and
        // in reality the outcome is order sensitive - a large repair that
        // comes after an fatal blast cannot save an already dead victim.
        // To simulate this we look at two points in the time chunk, and
        // average the damages/heals from these and take these as an
        // average representation of the overall repairs and heals across
        // time chunks. We want some hull damage to leak through, and
        // vessels with low armour buffers are still vulnerable even if
        // receiving large repairs. Repairs are modelled as being smoothly
        // received over the time chunk and damage as happening in
        // instants.
        const int total_health = hull_ + armour_;

        if( ( total_health + accumulated_armour_repair_ ) > accumulated_damage_ )
        {
            // Sample at t/4 and 3t/4. Armour increases across the time chunk t
            // so at t/4 it is 25% delivered, and at 3t/4 it is 75% delivered.
            // Damage arriving at t/4 will contend with a quarter healed armour
            // and damage arriving at 3t/4 with three quarters healed.
            // We are trying to find the average amount leaked through into
            // hull - as hull damage is irreparable.

            const int damage_exceeding_health = accumulated_damage_ - total_health;
            const int damage_then_repair = std::max( 0, accumulated_damage_ - ( armour_ + accumulated_armour_repair_ / 4 ) );
            const int repair_then_damage = std::max( 0, accumulated_damage_ - ( armour_ + 3 * accumulated_armour_repair_ / 4 ) );
            const int hull_damage   = ( damage_then_repair + repair_then_damage ) / 2;
            const int armour_damage = accumulated_damage_ - hull_damage;

            assert( armour_damage >= 0 );
            assert( hull_damage >= 0 );

            armour_ -= armour_damage;
            hull_ -= hull_damage;

            if( hull_ > 0 && damage_exceeding_health > 0 )
            {
                // There is the statistical probability that at some point
                // enough damaged arrived before enough repair was applied
                // such that the vessel died.
                // accumulated_damage_ - total_health = how far into "death" the
                // vessel went across the time unit. It had to be repaired by
                // at least this much to remain alive, and repairs increase
                // linearly across the time unit.
                const float probDeath = static_cast<float>( damage_exceeding_health ) / accumulated_armour_repair_;
                const float r = randoms.frand();
                if( r < probDeath )
                {
                    // Death...
                    armour_ = 0;
                    // We want negative hull. The more we failed the 'death test'
                    // by, the more the damage was.
                    hull_ = static_cast<int>( damage_exceeding_health * ( ( r / probDeath ) - 1.0f ) );
                }
            }
        }
        else
        {
            // Definite death - no need to do calculations.
            // Total damage exceeds all health + repairs = Death.
            // Try to keep the hull and armour reflecting the reality of the
            // damage, for statistics, not just set hull to zero. We know
            // however armour is zeroed.
            armour_ = 0;
            hull_ = total_health + accumulated_armour_repair_ - accumulated_damage_;
        }

        // Reset for next turn.
        accumulated_damage_ = 0;
        accumulated_armour_repair_ = 0;
    }
}
