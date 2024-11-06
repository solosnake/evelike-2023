#ifndef solosnake_ilanguagetext_hpp
#define solosnake_ilanguagetext_hpp

#include "solosnake/utf8text.hpp"

namespace solosnake
{
    //! This represents a mapping of a text id to a set of UTF-8 strings in a
    //! given language.
    //! By default if the key does not appear in the language set, then the key
    //! is returned as its own translation. This permits the English language
    //! keys to be also used as their translations.
    class ilanguagetext
    {
    public:

        virtual ~ilanguagetext();

        virtual utf8text text_for( const utf8text& ) const = 0;
    };
}

#endif
