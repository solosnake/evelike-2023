#include "solosnake/blue/blue_good_rand.hpp"

namespace blue
{
    inline double Good_rand::get_drand() noexcept
    {
        return distribution_(generator_);
    }

    inline float Good_rand::get_frand() noexcept
    {
        return static_cast<float>( distribution_(generator_) );
    }
}