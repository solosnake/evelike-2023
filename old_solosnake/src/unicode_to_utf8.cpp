#include "solosnake/unicode_to_utf8.hpp"
#include "solosnake/utf8text.hpp"
#include <cwchar>
#include <iterator>

namespace solosnake
{
    namespace
    {
        template <typename T> inline utf8text convert_to_utf8text( const std::basic_string<T>& s )
        {
            std::u16string s16;
            std::copy( s.cbegin(), s.cend(), std::back_inserter( s16 ) );
            return utf8text( s16 );
        }

        template <> inline utf8text convert_to_utf8text( const std::u16string& s )
        {
            return utf8text( s );
        }
    }

    std::string unicode_to_utf8( const std::wstring& unicode )
    {
        if( !unicode.empty() )
        {
            return convert_to_utf8text( unicode );
        }

        return std::string();
    }
}
