#ifndef blue_for_sale_hpp
#define blue_for_sale_hpp

#include <cstdint>

namespace blue
{
    /// Represents a quantity of something on sale for a given price.
    struct For_sale
    {
        For_sale() = default;

        For_sale( std::uint16_t p, std::uint16_t q ) noexcept
            : price( p ), quantity( q )
        {
        }

        std::uint16_t price    = 0u;
        std::uint16_t quantity = 0u;
    };
}

#endif
