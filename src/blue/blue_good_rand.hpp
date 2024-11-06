#ifndef blue_good_rand_hpp
#define blue_good_rand_hpp

#include <cstdint>
#include <memory>
#include <random>
#include "solosnake/blue/blue_randoms.hpp"

namespace blue
{
    /// Returns a Good_rand instance seeded with `seed`.
    std::unique_ptr<Randoms> make_good_rand(std::uint32_t seed);

    /// This should be a reproducible random number generator.
    class Good_rand final : public Randoms
    {
    public:

        explicit Good_rand(std::uint32_t seed);

        bool operator ==(const Good_rand& rhs) const noexcept;

    private:

        double get_drand() noexcept final;

        float  get_frand() noexcept final;

    private:

        std::uniform_real_distribution<double>  distribution_;
        std::mt19937                            generator_;
        unsigned int                            first_seed_;
    };
}

#include "solosnake/blue/blue_good_rand.inl"
#endif
