#include <ostream>
#include <iomanip>
#include "solosnake/blue/blue_instruction.hpp"

namespace blue
{
    namespace
    {
        constexpr std::string_view numbers   = "0123456789";
        constexpr std::string_view lowercase = "abcdefghijklmnopqrstuvwxyz";
        constexpr std::string_view uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        constexpr std::string_view other     = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

        inline bool letter_in(char x, const std::string_view& txt) noexcept
        {
            return std::string_view::npos != txt.find(x);
        }

        inline bool letter_is_text(char x) noexcept
        {
            return letter_in(x, lowercase)
                || letter_in(x, uppercase)
                || letter_in(x, numbers)
                || letter_in(x, other)
                ;
        }

        inline char to_ascii(char x) noexcept
        {
            return letter_is_text(x) ? x : '?';
        }

        std::array<char,2> arg_to_chars(const std::uint16_t a) noexcept
        {
            return std::array<char,2>{
                to_ascii( static_cast<char>(a & std::uint16_t(0x00FF)) ),
                to_ascii( static_cast<char>((a & std::uint16_t(0xFF00)) >> 8u) )
            };
        }
    }

    std::ostream& operator<<( std::ostream& os, const Instruction& i )
    {
        auto f = os.flags();

        os << std::setw( 4 ) << std::setfill( '0' ) << uppercase << std::hex << i[0] << ' ';
        os << std::setw( 4 ) << std::setfill( '0' ) << uppercase << std::hex << i[1] << ' ';
        os << std::setw( 4 ) << std::setfill( '0' ) << uppercase << std::hex << i[2] << ' ';
        os << std::setw( 4 ) << std::setfill( '0' ) << uppercase << std::hex << i[3];

        os.flags( f );

        return os;
    }

    bool Instruction::check_assert( std::uint16_t op1, std::uint16_t op2, std::uint16_t op )
    {
        bool result = false;

        std::uint8_t operation[2];

        operation[0] = static_cast<std::uint8_t>( 0x00FF & ( op ) );
        operation[1] = static_cast<std::uint8_t>( 0x00FF & ( op >> 8 ) );

        // && || < > & | ! == != <= >=
        if( op == 0 )
        {
            result = op1 != 0;
        }
        else if( operation[1] == '=' )
        {
            //
            if( operation[0] == '=' )
            {
                result = op1 == op2;
            }
            else if( operation[0] == '!' )
            {
                result = op1 != op2;
            }
            else if( operation[0] == '<' )
            {
                result = op1 <= op2;
            }
            else if( operation[0] == '>' )
            {
                result = op1 >= op2;
            }
            else
            {
              assert( !"Unsupported assert operator." );
            }
        }
        else if( operation[0] == '&' && operation[1] == '&' )
        {
            result = op1 && op2;
        }
        else if( operation[0] == '|' && operation[1] == '|' )
        {
            result = op1 || op2;
        }
        else if( operation[0] == '|' && operation[1] == 0 )
        {
            result = ( 0 != ( op1 | op2 ) );
        }
        else if( operation[0] == '&' && operation[1] == 0 )
        {
            result = ( 0 != ( op1 & op2 ) );
        }
        else if( operation[0] == '<' && operation[1] == 0 )
        {
            result = op1 < op2;
        }
        else if( operation[0] == '>' && operation[1] == 0 )
        {
            result = op1 > op2;
        }
        else if( operation[0] == '!' && operation[1] == 0 )
        {
            result = !op1;
        }
        else
        {
            assert( !"Unsupported assert operator." );
        }

        return result;
    }

    std::array<char,6> Instruction::args_as_chars() const noexcept
    {
        std::array<char,6> txt;
        auto a0 = arg_to_chars( args_[0] );
        auto a1 = arg_to_chars( args_[1] );
        auto a2 = arg_to_chars( args_[2] );
        txt[0] = a0[0];
        txt[1] = a0[1];
        txt[2] = a1[0];
        txt[3] = a1[1];
        txt[4] = a2[0];
        txt[5] = a2[1];
        return txt;
    }

    std::string Instruction::args_as_string() const
    {
        std::string txt;
        txt.resize(7);
        auto chars = args_as_chars();
        txt[0] = chars[0];
        txt[1] = chars[1];
        txt[2] = chars[0];
        txt[3] = chars[1];
        txt[4] = chars[0];
        txt[5] = chars[1];
        return txt;
    }
}
