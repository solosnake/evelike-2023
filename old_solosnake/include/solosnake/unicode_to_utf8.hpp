#ifndef solosnake_unicode_to_utf8_hpp
#define solosnake_unicode_to_utf8_hpp

#include <string>

namespace solosnake
{
    //! Returns the given null terminated unicode string converted
    //! to UTF-8. Returns the empty string if the string was unable
    //! to be converted.
    std::string unicode_to_utf8( const std::wstring& unicodeString );
}

#endif
