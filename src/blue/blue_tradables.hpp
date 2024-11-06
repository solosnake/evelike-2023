#ifndef blue_tradables_hpp
#define blue_tradables_hpp

#include <array>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include "solosnake/blue/blue_tradabletypes.hpp"

namespace blue
{
    /// Represents an amount of Tradables, a sum, a cost, not real goods.
    class Amount
    {
    public:

        Amount() = default;

        explicit Amount( const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT>& );

        Amount( TradableTypes t, const std::int16_t units );

        Amount( OreTypes t, const std::int16_t units );

        std::uint16_t volume() const;

        std::int16_t operator[]( const size_t n ) const;

        std::int16_t& operator[]( const size_t n );

        Amount& operator+=( const Amount& );

        Amount operator+( const Amount& rhs ) const;

        bool operator == ( const Amount& rhs ) const noexcept;
        bool operator != ( const Amount& rhs ) const noexcept;

    private:

        std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT> amounts_ = {};
    };

    /// An array of all types of tradable commodities, and the number of them
    /// contained by this object. This object represents a collection of
    /// "monies", a tangible amount of "things". A Tradables object can
    /// represent a cost of something, or the amount of things needed to build
    /// an item etc. It cannot represent a negative amount, or a debt.
    class Tradables
    {
    public:

        Tradables();

        explicit Tradables( const Amount& );

        bool can_afford( const Amount& ) const;

        void debit( const Amount& );

        void credit( const Amount& );

        std::uint16_t volume() const;

        Amount as_amount() const;

        std::int16_t operator[]( const size_t n ) const;

        std::int16_t& operator[]( const size_t n );

        static std::uint16_t volume_of( const std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT>& );

        bool operator == ( const Tradables& rhs ) const noexcept;
        bool operator != ( const Tradables& rhs ) const noexcept;

    private:

        /// Max and min amount is -32767 to +32767
        std::array<std::int16_t, BLUE_TRADABLE_TYPES_COUNT> amounts_ = {};
    };
}

#include "solosnake/blue/blue_tradables.inl"
#endif
