#ifndef blue_team_hpp
#define blue_team_hpp

#include "solosnake/blue/blue_bgr.hpp"

namespace blue
{
    /// Each entity on the board has an associated team.
    class team
    {
    public:

        Bgr colour() const
        {
            return colour_;
        }

    private:

        Bgr colour_;
    };

    inline bool operator == (const team& lhs, const team& rhs) noexcept
    {
        return lhs.colour() == rhs.colour();
    }
}

#endif
