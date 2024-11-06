#include "solosnake/blue/blue_component.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Component::Component(const std::string_view& name,
                         const std::int32_t hull_hitpoints,
                         const std::int32_t mass,
                         const ComponentCategory categorisation,
                         const Amount build_cost,
                         const float build_complexity)
        : component_name_(name)
        , hull_hit_points_(hull_hitpoints)
        , mass_(mass)
        , categorisation_(categorisation)
        , build_cost_(build_cost)
        , build_complexity_(build_complexity)
    {
        if( build_complexity_ < 0.0f )
        {
            ss_throw( "Invalid complexity." );
        }

        if( component_name_.empty() )
        {
            ss_throw( "component name is empty." );
        }

        if( hull_hit_points_ <= 0 )
        {
            ss_throw( "component 'hull_hitpoints' is invalid, must be greater than zero." );
        }

        if( mass_ <= 0 )
        {
            ss_throw( "component 'mass' is invalid, must be greater than zero." );
        }
    }

    bool Component::operator == (const Component& rhs) const noexcept
    {
        return (component_name_ == rhs.component_name_)
            && (hull_hit_points_ == rhs.hull_hit_points_)
            && (mass_ == rhs.mass_)
            && (categorisation_ == rhs.categorisation_)
            && (build_cost_ == rhs.build_cost_)
            && (build_complexity_ == rhs.build_complexity_)
            ;
    }

    bool Component::operator != (const Component& rhs) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
