#include "solosnake/blue/blue_instructions.hpp"
#include <cassert>

namespace blue
{
    inline Instruction& Instructions::operator[]( size_t n )
    {
        assert( n > 0 );
        assert( n <= instructions_.size() );
        assert( !instructions_.empty() );
#ifdef NDEBUG
        return instructions_[( n + instructions_.size() - 1 ) % instructions_.size()];
#else
        return instructions_.at(( n + instructions_.size() - 1 ) % instructions_.size());
#endif
    }

    inline const Instruction& Instructions::operator[]( size_t n ) const
    {
        assert( n > 0 );
        assert( n <= instructions_.size() );
        assert( !instructions_.empty() );
#ifdef NDEBUG
        return instructions_[( n + instructions_.size() - 1 ) % instructions_.size()];
#else
        return instructions_.at(( n + instructions_.size() - 1 ) % instructions_.size());
#endif
    }

    inline bool Instructions::operator==( const Instructions& rhs ) const noexcept
    {
        return instructions_ == rhs.instructions_;
    }

    inline bool Instructions::operator!=( const Instructions& rhs ) const noexcept
    {
        return instructions_ != rhs.instructions_;
    }

    inline std::uint16_t Instructions::size() const noexcept
    {
        return static_cast<std::uint16_t>( instructions_.size() );
    }

    inline void Instructions::swap( Instructions& rhs ) noexcept
    {
        instructions_.swap( rhs.instructions_ );
    }

    inline Instructions Instructions::subset( size_t start, size_t n ) const
    {
        assert( start > 0 );
        assert( n > 0 );
        assert( ( start - 1 ) < size() );
        assert( ( start + n - 1 ) <= size() );

        return Instructions(
                   std::vector<Instruction>( instructions_.cbegin() + ( start - 1 ),
                                             instructions_.cbegin() + ( start + n - 1 ) ) );
    }
}
