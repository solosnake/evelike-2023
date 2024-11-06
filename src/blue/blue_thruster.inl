#include "solosnake/blue/blue_thruster.hpp"

namespace blue
{
    inline std::int32_t Thruster::power() const noexcept
    {
        return engine_power_;
    }

    inline const Component& Thruster::details() const noexcept
    {
        return component_;
    }
}