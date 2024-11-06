#ifndef blue_randoms_hpp
#define blue_randoms_hpp

#include <cstdint>
#include <memory>

namespace blue
{
    /// Pseudo random number interface.
    class Randoms
    {
    public:

        virtual ~Randoms() = default;

        /// Returns a pseudo-random double between 0.0 and 1.0 inclusive.
        double drand() noexcept;

        /// Returns a pseudo-random float between 0.0f and 1.0f inclusive.
        float frand() noexcept;

        /// Returns a pseudo random unsigned integer between 0 and 0xFFFFFFFF inclusive.
        std::uint32_t urand() noexcept;

    protected:

        /// Returns a pseudo-random float between 0.0f and 1.0f inclusive.
        virtual double get_drand() noexcept = 0;

        /// Returns a pseudo-random float between 0.0f and 1.0f inclusive.
        virtual float get_frand() noexcept = 0;
    };
}

#include "solosnake/blue/blue_randoms.inl"
#endif
