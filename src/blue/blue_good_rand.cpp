#include "solosnake/blue/blue_good_rand.hpp"
#include <random>

namespace blue
{
    Good_rand::Good_rand(std::uint32_t seed)
        : distribution_(0.0, 1.0)
        , generator_(seed)
        , first_seed_(seed)
    {
    }

    bool Good_rand::operator ==(const Good_rand& rhs) const noexcept
    {
        return distribution_ == rhs.distribution_
            && generator_    == rhs.generator_
            && first_seed_   == rhs.first_seed_
            ;
    }

    std::unique_ptr<Randoms> make_good_rand(unsigned int seed)
    {
        return std::make_unique<Good_rand>( seed );
    }
}
