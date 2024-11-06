#ifndef blue_hardpoint_shooter_hpp
#define blue_hardpoint_shooter_hpp

#include <cstdint>
#include "solosnake/blue/blue_combat_system.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hardpointtype.hpp"
#include "solosnake/blue/blue_randoms.hpp"

namespace blue
{
    /// The workings of a Hardpoint: the part that does the shooting, and
    /// which is not directly related to being a Component. This class is
    /// responsible for calculating the shot damage in combat/repairing.
    ///
    /// A weapon's damage is its damage at minimum range, which is always
    /// considered 100%. Weapons that do more damage at max range will have a
    /// falloff which is > 100%, meaning their minimum range damage is amplified
    /// at max range. Weapons commonly do less damage at max range: these will
    /// have a falloff less than 100%, and the damage will range between 100% at
    /// min range to falloff% at max range.
    ///
    /// The effect of a shot is determined by range, falloff and the weapon's
    /// reliability, and a normal distribution of damage within the 'unreliable'
    /// part. A reliability of 100% effectively removes the random effect from
    /// a shot: a 100% reliable weapon will always deliver its max potential
    /// damage for the given target range.
    ///
    /// All Hardpoint_shooters must have turn speeds above zero. Fixed position
    /// Hardpoint_shooters are created by having fixed min and max angle, not by
    /// preventing movement.
    class Hardpoint_shooter
    {
    public:

        Hardpoint_shooter(
            const Hardpoint_shot& hardpointshot,
            const Fixed_angle     rotation_speed,  // Per frame
            const std::uint16_t   firing_cap_cost,
            const std::uint16_t   firing_cooldown,  // In frames.
            const std::int16_t    min_angle_degrees,
            const std::int16_t    max_angle_degrees,
            const std::uint16_t   min_range,
            const std::uint16_t   max_range_percent_effect,  // 0-100+ %, factor applied to damage at max range.
            const std::uint8_t    reliability // 0-100%
        );

        void set_index( const size_t index );

        std::uint16_t firing_cap_cost() const noexcept;

        std::uint16_t firing_cooldown() const noexcept;

        std::uint16_t min_range() const noexcept;

        std::uint16_t max_range() const noexcept;

        Fixed_angle  min_angle() const noexcept;

        Fixed_angle  max_angle() const noexcept;

        Fixed_angle  shooter_rotation_speed() const noexcept;

        HardpointType hardpoint_type() const noexcept;

        Hardpoint_shot make_a_shot( Randoms& ) const noexcept;

        float get_shooter_falloff_at_range( std::uint16_t range ) const noexcept;

        /// Returns the (Fixed_angle) angle of the shooter, midway between
        /// its min and max angles.
        Fixed_angle get_mid_angle() const noexcept;

        bool operator == ( const Hardpoint_shooter& ) const noexcept;
        bool operator != ( const Hardpoint_shooter& ) const noexcept;

        static float calculate_falloff( std::uint16_t target_range,
                                        std::uint16_t min_range,
                                        std::uint16_t max_range,
                                        std::uint16_t max_range_effect ) noexcept;
    private:

        void validate() const;

    private:

        // Allows serialisation to read the members.
        friend class to_from_json;

    private:
        Hardpoint_shot  hardpoint_shot_;
        Fixed_angle     rotation_speed_;            // Per frame, must be > zero.
        std::uint16_t   firing_cap_cost_;
        std::uint16_t   firing_cooldown_;           // In frames.
        std::int16_t    min_angle_degrees_;         // Converted to Fixed_angle in `min_angle()`.
        std::int16_t    max_angle_degrees_;         // Converted to Fixed_angle in `max_angle()`.
        std::uint16_t   min_range_;                 // Max range is in hardpoint_shot_.
        std::uint16_t   max_range_percent_effect_;  // 0-100+ %, factor applied to damage at max range.
        std::uint8_t    reliability_;               // 0-100 %. 100% reliability effectively negates randomness.
    };
}

#include "solosnake/blue/blue_hardpoint_shooter.inl"
#endif
