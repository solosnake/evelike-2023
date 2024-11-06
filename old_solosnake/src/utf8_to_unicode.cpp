#include "solosnake/utf8_to_unicode.hpp"
#include "solosnake/utf8text.hpp"
#include <cwchar>
#include <iterator>

using namespace std;

namespace solosnake
{
    namespace
    {
        template <typename T> inline wstring convert_to_wstring( const basic_string<T>& txt )
        {
            wstring wtxt;
            copy( txt.cbegin(), txt.cend(), back_inserter( wtxt ) );
            return wtxt;
        }

        template <>
        inline wstring convert_to_wstring<wstring::value_type>( const basic_string<wchar_t>& txt )
        {
            return txt;
        }
    }

    wstring utf8_to_unicode( const string& utf8 )
    {
        if( !utf8.empty() )
        {
            utf8text txt8( utf8 );
            return convert_to_wstring( txt8.to_utf16() );
        }

        return wstring();
    }
}
