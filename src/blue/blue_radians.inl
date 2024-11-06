#include "solosnake/blue/blue_radians.hpp"
#include <cmath>

namespace blue
{
    inline Radians::Radians(const float r) noexcept
        : value_(r)
    {
    }

    inline float Radians::sin() const noexcept
    {
        return std::sin(value_);
    }

    inline float Radians::cos() const noexcept
    {
        return std::cos(value_);
    }

    inline float Radians::tan() const noexcept
    {
        return std::tan(value_);
    }

    inline bool Radians::operator==(Radians rhs) const noexcept
    {
        return value_ == rhs.value_;
    }

    inline bool Radians::operator!=(Radians rhs) const noexcept
    {
        return value_ != rhs.value_;
    }

    inline float Radians::value() const noexcept
    {
        return value_;
    }

    inline Radians::operator float() const noexcept
    {
        return value_;
    }
}
