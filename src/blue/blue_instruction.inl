#include "solosnake/blue/blue_instruction.hpp"
#include <cassert>
#include <stdexcept>

namespace blue
{
    inline constexpr std::uint16_t Instruction::cast( unsigned int n ) noexcept
    {
        return static_cast<std::uint16_t>( n );
    }

    inline Instruction::Instruction(
        std::uint16_t cmd_with_flags,
        std::uint16_t a,
        std::uint16_t b,
        std::uint16_t c ) noexcept
    {
        command_ = cmd_with_flags;
        args_[0] = a;
        args_[1] = b;
        args_[2] = c;
    }

    inline bool Instruction::is_indirection( unsigned int n ) const noexcept
    {
        assert( n < 3 );
        return 0 == ( command_ & static_cast<std::uint16_t>( BLUE_VALUE_ARG0 << n ) );
    }

    inline std::uint16_t Instruction::cmd() const noexcept
    {
        return command_ & BLUE_INSTRUCTION_MASK;
    }

    inline std::uint16_t Instruction::flags() const noexcept
    {
        return command_ & BLUE_VALUE_ARG_MASK;
    }

    inline std::uint16_t Instruction::arg( unsigned int n ) const noexcept
    {
        assert( n < 3 );
        return args_[n];
    }

    inline std::uint16_t& Instruction::arg( unsigned int n ) noexcept
    {
        assert( n < 3 );
        return args_[n];
    }

    inline std::uint16_t Instruction::operator[]( size_t n ) const noexcept
    {
        return n == 0 ? command_ : args_[n - 1];
    }

    inline std::vector<Instruction> Instruction::asm_to_instructions( const std::vector<std::uint16_t>& in )
    {
        std::vector<Instruction> results;

        if( ! in.empty() )
        {
            if( 0 != in.size() % 4 )
            {
                throw std::runtime_error( "Incoming 'asm' is not multiple of 4." );
            }

            const std::uint16_t* p = &in[0];
            const size_t n = in.size() / 4;
            results.reserve( n );
            for( size_t i = 0; i < n; ++i )
            {
                results.push_back( Instruction::from_asm( p[0], p[1], p[2], p[3] ) );
                p += 4;
            }
        }

        return results;
    }

    inline std::vector<Instruction> Instruction::asm_to_instructions( const std::uint16_t* unaliased p, size_t n )
    {
        std::vector<Instruction> results;
        results.reserve( n );
        for( size_t i = 0; i < n; ++i )
        {
            results.push_back( Instruction::from_asm( p[0], p[1], p[2], p[3] ) );
            p += 4;
        }
        return results;
    }

    inline Instruction Instruction::from_asm(
        std::uint16_t cmd_with_flags,
        std::uint16_t a,
        std::uint16_t b,
        std::uint16_t c )
    {
        return Instruction( cmd_with_flags, a, b, c );
    }

    inline bool Instruction::operator == ( const Instruction& rhs ) const noexcept
    {
        return
            command_ == rhs.command_ &&
            args_[0] == rhs.args_[0] &&
            args_[1] == rhs.args_[1] &&
            args_[2] == rhs.args_[2];
    }

    inline bool Instruction::operator != ( const Instruction& rhs ) const noexcept
    {
        return !( *this == rhs );
    }

    inline Instruction Instruction::value( std::uint16_t what )
    {
        return Instruction( Instruction::cast( BLUE_ASM_NOP ), Instruction::cast( what ) );
    }

    inline Instruction Instruction::location( std::uint8_t x, std::uint8_t y )
    {
        std::uint16_t xy16 = ( ( 0xFF00 & std::uint16_t( x ) << 8 ) | ( std::uint16_t( y ) & 0x00FF ) );
        return Instruction( Instruction::cast( BLUE_ASM_LOCATION ), xy16 );
    }

    inline Instruction Instruction::nop()
    {
        return Instruction::value( 0 );
    }

    inline bool Instruction::assert_op_to_str( std::uint16_t op, char s[3] )
    {
        s[0] = static_cast<char>( 0x00FF & ( op ) );
        s[1] = static_cast<char>( 0x00FF & ( op >> 8 ) );
        return !( op == 0 || ( s[0] == '!' && s[1] == 0 ) );
    }
}