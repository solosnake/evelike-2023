#include "solosnake/blue/blue_randoms.hpp"

namespace blue
{
    inline double Randoms::drand() noexcept
    {
        return get_drand();
    }

    inline float Randoms::frand() noexcept
    {
        return get_frand();
    }

    inline std::uint32_t Randoms::urand() noexcept
    {
        return static_cast<std::uint32_t>( drand() * 0xFFFFFFFF );
    }
}
