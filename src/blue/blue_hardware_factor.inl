#include "solosnake/blue/blue_hardware_factor.hpp"

namespace blue
{
    inline Hardware_factor::operator float() const noexcept
    {
        return value_;
    }

    inline Hardware_factor& Hardware_factor::operator*=(const Hardware_factor &rhs) noexcept
    {
        value_ *= rhs.value_;
        return *this;
    }

    inline bool Hardware_factor::operator==(const Hardware_factor &rhs) const noexcept
    {
        return value_ == rhs.value_;
    }

    inline bool Hardware_factor::operator!=(const Hardware_factor &rhs) const noexcept
    {
        return value_ != rhs.value_;
    }
}