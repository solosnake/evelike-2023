#include "solosnake/blue/blue_thruster.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Thruster::Thruster(const Component& c, std::int32_t engine_power)
        : component_(c)
        , engine_power_(engine_power)
    {
        if(c.categorisation() != PropulsionComponent)
        {
            ss_throw("Invalid thruster categorisation.");
        }
    }

    bool Thruster::operator== (const Thruster& rhs) const noexcept
    {
        return component_    == rhs.component_
            && engine_power_ == rhs.engine_power_;
    }

    bool Thruster::operator!= (const Thruster& rhs) const noexcept
    {
        return ! ((*this) == rhs);
    }
}
