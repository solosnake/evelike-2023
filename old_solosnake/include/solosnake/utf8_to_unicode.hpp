#ifndef solosnake_utf8_to_unicode_hpp
#define solosnake_utf8_to_unicode_hpp

#include <string>

namespace solosnake
{
    //! Returns the given null terminated UTF-8 string as a wide char
    //! null terminated unicode string.
    //! Returns the empty string if the given string was unable to
    //! be converted.
    std::wstring utf8_to_unicode( const std::string& );
}

#endif
