#include "solosnake/blue/blue_blueprint.hpp"

namespace blue
{
    inline const Hardware& Blueprint::hardware_of_blueprint() const noexcept
    {
        return hardware_;
    }

    inline Amount Blueprint::build_cost() const noexcept
    {
        return build_cost_;
    }

    inline float Blueprint::build_complexity() const noexcept
    {
        return build_complexity_;
    }

    inline size_t Blueprint::hardpoint_count() const noexcept
    {
        return hardpoints_.size();
    }

    inline size_t Blueprint::softpoint_count() const noexcept
    {
        return softpoints_.size();
    }

    inline size_t Blueprint::thrusters_count() const noexcept
    {
        return thrusters_.size();
    }

    inline size_t Blueprint::component_count() const noexcept
    {
        return hardpoint_count() + softpoint_count() + thrusters_count();
    }

    inline const Hardpoint& Blueprint::hardpoint_at( size_t n ) const
    {
        assert( n < hardpoints_.size() );
        return hardpoints_.at(n).first.oriented_hardpoint();
    }

    inline const Softpoint& Blueprint::softpoint_at( size_t n ) const
    {
        assert( n < softpoints_.size() );
        return softpoints_.at(n).first;
    }

    inline const Thruster& Blueprint::thruster_at( size_t n ) const
    {
        assert( n < thrusters_.size() );
        return thrusters_.at(n).first;
    }

    inline AttachPoint Blueprint::hardpoint_attachpoint( size_t n ) const
    {
        assert( n < hardpoints_.size() );
        return hardpoints_.at(n).second;
    }

    inline AttachPoint Blueprint::softpoint_attachpoint( size_t n ) const
    {
        assert( n < softpoints_.size() );
        return softpoints_.at(n).second;
    }

    inline Thruster_attachpoint Blueprint::thruster_attachpoint( size_t n ) const
    {
        assert( n < thrusters_.size() );
        return thrusters_.at(n).second;
    }

    inline const Chassis& Blueprint::blueprint_chassis() const noexcept
    {
        return chassis_;
    }
}
