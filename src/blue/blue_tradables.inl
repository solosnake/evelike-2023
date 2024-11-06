#include "solosnake/blue/blue_tradables.hpp"

namespace blue
{
    inline std::int16_t Amount::operator[]( const size_t n ) const
    {
        assert( n < BLUE_TRADABLE_TYPES_COUNT );
        return amounts_[n];
    }

    inline std::int16_t& Amount::operator[]( const size_t n )
    {
        assert( n < BLUE_TRADABLE_TYPES_COUNT );
        return amounts_[n];
    }

    inline std::uint16_t Amount::volume() const
    {
        return Tradables::volume_of( amounts_ );
    }

    inline bool Amount::operator == ( const Amount& rhs ) const noexcept
    {
        return amounts_ == rhs.amounts_;
    }

    inline bool Amount::operator != ( const Amount& rhs ) const noexcept
    {
        return amounts_ != rhs.amounts_;
    }

    //-------------------------------------------------------------------------

    inline Tradables::Tradables() : amounts_{ 0, 0, 0, 0, 0, 0, 0, 0 }
    {
    }

    inline std::int16_t Tradables::operator[]( const size_t n ) const
    {
        assert( n < BLUE_TRADABLE_TYPES_COUNT );
        return amounts_[n];
    }

    inline std::int16_t& Tradables::operator[]( const size_t n )
    {
        assert( n < BLUE_TRADABLE_TYPES_COUNT );
        return amounts_[n];
    }

    inline std::uint16_t Tradables::volume() const
    {
        return Tradables::volume_of( amounts_ );
    }

    inline Amount Tradables::as_amount() const
    {
        return Amount( amounts_ );
    }

    inline bool Tradables::operator == ( const Tradables& rhs ) const noexcept
    {
        return amounts_ == rhs.amounts_;
    }

    inline bool Tradables::operator != ( const Tradables& rhs ) const noexcept
    {
        return amounts_ != rhs.amounts_;
    }
}
