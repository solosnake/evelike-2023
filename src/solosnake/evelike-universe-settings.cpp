#include "solosnake/evelike-universe-settings.hpp"

namespace solosnake::evelike
{
    bool Universe_settings::operator == (const Universe_settings&) const noexcept
    {
        return true;
    }

    bool Universe_settings::operator != (const Universe_settings& u) const noexcept
    {
        return not (*this == u);
    }
}