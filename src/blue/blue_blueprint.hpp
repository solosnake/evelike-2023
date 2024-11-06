#ifndef blue_blueprint_hpp
#define blue_blueprint_hpp

#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include <utility>
#include "solosnake/blue/blue_attachpoint.hpp"
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_game_logic_constants.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_hardware.hpp"
#include "solosnake/blue/blue_oriented_hardpoint.hpp"
#include "solosnake/blue/blue_result.hpp"
#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_thruster_attachpoint.hpp"
#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    class Hardware_modifier;

    /// A Blueprint is required to build a Machine. This is an immutable class
    /// and a Machine contains a shared pointer to its Blueprint.
    class Blueprint
    {
    public:

        static Result can_form_valid_blueprint(
            const Chassis&,
            const std::map<Thruster_attachpoint, Thruster>&,
            const std::map<AttachPoint, Softpoint>&,
            const std::map<AttachPoint, Oriented_hardpoint>& );

        Blueprint(
            const Chassis&,
            const std::map<Thruster_attachpoint, Thruster>&,
            const std::map<AttachPoint, Softpoint>&,
            const std::map<AttachPoint, Oriented_hardpoint>& );

        /// How much material is required to construct an instance of this Blueprint.
        Amount build_cost() const noexcept;

        /// Returns the Hardware this Blueprint creates.
        const Hardware& hardware_of_blueprint() const noexcept;

        /// A measure of how complex or high-end the Component this Blueprint
        /// makes is. Zero means simple. Higher factors slow build times.
        float build_complexity() const noexcept;

        size_t hardpoint_count() const noexcept ;

        size_t softpoint_count() const noexcept;

        size_t thrusters_count() const noexcept;

        size_t component_count() const noexcept;

        const Chassis& blueprint_chassis() const noexcept;

        const Hardpoint& hardpoint_at( size_t n ) const;

        const Softpoint& softpoint_at( size_t n ) const;

        const Thruster& thruster_at( size_t n ) const;

        Fixed_angle hardpoint_angle( size_t n ) const;

        AttachPoint hardpoint_attachpoint( size_t n ) const;

        AttachPoint softpoint_attachpoint( size_t n ) const;

        Thruster_attachpoint thruster_attachpoint( size_t n ) const;
        bool operator == ( const Blueprint& rhs ) const noexcept;
        bool operator != ( const Blueprint& rhs ) const noexcept;

        /// Maximum number of permitted machine parts on any one Machine.
        static const size_t MaxComponents = BLUE_MAX_MACHINE_COMPONENTS;

    private:

        Hardware          calculate_hardware() const;

        Hardware_modifier cumulative_modifiers() const;

    private:

        friend class to_from_json;
        std::map<Thruster_attachpoint, Thruster>  make_thrustpoints_map() const;
        std::map<AttachPoint, Softpoint>          make_softpoints_map() const;
        std::map<AttachPoint, Oriented_hardpoint> make_oriented_hardpoints_map() const;

    private:

        using Attached_hardpoint = std::pair<Oriented_hardpoint, AttachPoint>;
        using Attached_softpoint = std::pair<Softpoint,          AttachPoint>;
        using Attached_thruster  = std::pair<Thruster,           Thruster_attachpoint>;

    private:

        Chassis                          chassis_;
        std::vector<Attached_hardpoint>  hardpoints_;
        std::vector<Attached_softpoint>  softpoints_;
        std::vector<Attached_thruster>   thrusters_;
        Amount                           build_cost_;
        float                            build_complexity_;
        Hardware                         hardware_;
    };
}

#include "solosnake/blue/blue_blueprint.inl"
#endif
