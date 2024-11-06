#include "solosnake/blue/blue_half_line_3d.hpp"

namespace blue
{
    inline Half_line_3d::Half_line_3d(const Point3d &p0, const Point3d &p1)
        : start_(p0), dir_(p1)
    {
        normalize();
    }

    inline const Point3d& Half_line_3d::head() const noexcept
    {
        return start_;
    }

    inline Point3d Half_line_3d::tail() const noexcept
    {
        return start_ + dir_;
    }

    inline const Point3d& Half_line_3d::direction() const noexcept
    {
        return dir_;
    }

    inline bool Half_line_3d::operator==(const Half_line_3d &rhs) const noexcept
    {
        return start_ == rhs.start_;
    }

    inline bool Half_line_3d::operator!=(const Half_line_3d &rhs) const noexcept
    {
        return start_ != rhs.start_;
    }
}
