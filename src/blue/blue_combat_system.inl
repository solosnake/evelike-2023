#include "solosnake/blue/blue_combat_system.hpp"

namespace blue
{

    inline float Damages::kinetic_dmg() const noexcept
    {
        return dmg_type[Combat_system::Kinetic];
    }

    inline float Damages::thermal_dmg() const noexcept
    {
        return dmg_type[Combat_system::Thermal];
    }

    inline float Damages::em_dmg() const noexcept
    {
        return dmg_type[Combat_system::ElectroMagnetic];
    }

    inline float Damages::radiation_dmg() const noexcept
    {
        return dmg_type[Combat_system::Radiation];
    }

    inline float Resistances::kinetic_resist() const noexcept
    {
        return resistance[Combat_system::Kinetic];
    }

    inline float Resistances::thermal_resist() const noexcept
    {
        return resistance[Combat_system::Thermal];
    }

    inline float Resistances::em_resist() const noexcept
    {
        return resistance[Combat_system::ElectroMagnetic];
    }

    inline float Resistances::radiation_resist() const noexcept
    {
        return resistance[Combat_system::Radiation];
    }

    inline Damages Damages::operator*(const float f) const noexcept
    {
        Damages d;
        d.dmg_type[0] = dmg_type[0] * f;
        d.dmg_type[1] = dmg_type[1] * f;
        d.dmg_type[2] = dmg_type[2] * f;
        d.dmg_type[3] = dmg_type[3] * f;
        return d;
    }

    inline Damages &Damages::operator*=(const float f) noexcept
    {
        dmg_type[0] *= f;
        dmg_type[1] *= f;
        dmg_type[2] *= f;
        dmg_type[3] *= f;
        return *this;
    }

    inline bool Damages::operator==(const Damages &rhs) const noexcept
    {
        return dmg_type[0] == rhs.dmg_type[0] && dmg_type[1] == rhs.dmg_type[1] && dmg_type[2] == rhs.dmg_type[2] && dmg_type[3] == rhs.dmg_type[3];
    }

    inline void Combat_system::apply_damage(const Damages &d) noexcept
    {
        accumulated_damage_ +=
            static_cast<int>(d.dmg_type[0] * resistance_.resistance[0] +
                             d.dmg_type[1] * resistance_.resistance[1] +
                             d.dmg_type[2] * resistance_.resistance[2] +
                             d.dmg_type[3] * resistance_.resistance[3]);
    }

    inline void Combat_system::repair_armour(int h) noexcept
    {
        assert(h >= 0);
        // Repairs are capped at max amour. A single repair can never
        // repair more than 100% of the armour.
        accumulated_armour_repair_ += std::min(h, total_armour_);
    }

    inline bool Combat_system::is_alive() const noexcept
    {
        return hull_ > 0;
    }

    inline void Combat_system::kill() noexcept
    {
        hull_ = 0;
    }

    inline bool Combat_system::resolve(Randoms &randoms)
    {
        // Check to see if any combat happened.
        if (accumulated_armour_repair_ || accumulated_damage_)
        {
            resolve_combat(randoms);
        }

        return is_alive();
    }
}
