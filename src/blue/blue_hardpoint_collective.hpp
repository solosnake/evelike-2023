#ifndef blue_hardpoint_collective_hpp
#define blue_hardpoint_collective_hpp

#include <memory>
#include <utility>
#include <vector>
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_turning_hardpoint.hpp"

namespace blue
{
    class Blueprint;
    class Hardpoint;
    class Hardware;
    class Randoms;

    /// Represents a set of Hardpoints in a Machine, acting as a single
    /// block (even if they do not actually synchronize very well).
    /// This is essentially a sub-Component class of a Machine.
    class Hardpoint_collective
    {
    public:

        Hardpoint_collective();

        Hardpoint_collective(
            const Hardware&,
            const Blueprint& );

        std::uint16_t max_range() const noexcept;

        std::uint16_t min_range() const noexcept;

        void tick();

        /// Returns true or false if this coordinate can be aimed on by any
        /// of the hardpoints. Aiming at self returns false.
        bool can_aim_at( Hex_coord );

        /// Returns the least number of hardpoints that can fire on this
        /// grid coordinate. Firing at self returns false.
        unsigned int can_fire_at(
            const std::uint16_t cap,
            const Hex_coord ) const;

        /// Aiming at self is prevented here.
        void aim_at( Hex_coord );

        /// Firing at self is prevented here.
        const std::vector<Hardpoint_shot>& fire_at(
            Randoms&,
            std::uint16_t& cap,
            const Hex_coord );

        float get_indexed_hardpoint_falloff_at_range(
            const size_t i,
            const std::uint16_t range ) const;

        void change_location( const Hex_coord ) noexcept;

        void change_orientation( const Fixed_angle ) noexcept;

        const std::vector<Turning_hardpoint>& turning_hardpoints() const noexcept;

    private:

        void tick_aim();

        void tick_cooldown();

    private:

        std::vector<Hardpoint_shot>     shots_fired_;
        std::vector<Turning_hardpoint>  hardpoints_;
        std::vector<std::uint8_t>       hardpoint_indices_by_cap_cost_;
        Fixed_angle                     target_angle_; // Angle to aim towards.
        Fixed_angle                     machine_facing_angle_;
        Hex_coord                       machine_location_;
        Hex_coord                       target_location_;
        std::uint16_t                   min_range_;
        std::uint16_t                   max_range_;
        bool                            aiming_;
        bool                            cooling_;
    };
}

#include "solosnake/blue/blue_hardpoint_collective.inl"
#endif
