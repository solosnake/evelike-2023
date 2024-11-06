#include <sstream>
#include "solosnake/bgra.hpp"
#include "solosnake/bgr.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
       // DUPLICATED CODE!! USE strtol!! C++11
        uint32_t string_to_uint32( const string& txt )
        {
            // Look for leading '0x' or '0X'.
            bool isHex = ( txt.length() > 2 ) && ( '0' == txt[0] ) && ( 'x' == txt[1] || 'X' == txt[1] );

            uint32_t c = 0;
            istringstream ss( txt );

            if( isHex )
            {
                if( !( ss >> hex >> c ) )
                {
                    ss_throw( "Could not convert string to uint32_t." );
                }
            }
            else
            {
                if( !( ss >> dec >> c ) )
                {
                    ss_err( "Could not convert string \"", txt, "\" to bgra." );
                    ss_throw( "Could not convert string to uint32_t." );
                }
            }

            return c;
        }
    }

    //! Constructs from uint32 colour as string, either base 10 or 16.
    //! @code
    //! bgra myColour( "0xFF00AA42" );
    //! bgra white( "0xFFFFFFFF" );
    //! bgra red( "0x0000FFFF" );
    //! bgra red1( "65535" );
    //! assert( red1 == red );
    //! @endcode
    bgra::bgra( const string& txt )
    {
        const auto c = string_to_uint32( txt );

        bgr_ = bgr( static_cast<uint8_t>( ( c & 0xFF000000 ) >> 24 ),
                    static_cast<uint8_t>( ( c & 0x00FF0000 ) >> 16 ),
                    static_cast<uint8_t>( ( c & 0x0000FF00 ) >>  8 ) );

        alpha_ = static_cast<uint8_t>( c & 0x000000FF );
    }

    uint32_t bgra::to_uint32() const
    {
        return bgr_.to_uint32() | static_cast<uint32_t>(alpha_);
    }
}
