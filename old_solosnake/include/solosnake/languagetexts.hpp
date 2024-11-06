#ifndef solosnake_languagetexts_hpp
#define solosnake_languagetexts_hpp

#include <map>
#include "solosnake/ilanguagetext.hpp"
#include "solosnake/utf8text.hpp"

namespace solosnake
{
    //! This represents a mapping of a text id to a set of UTF-8 strings in a given
    //! language.
    //! By default if the key does not appear in the language set, then the key is
    //! returned as its own translation. This permits the English language keys
    //! to be also used as their translations.
    class languagetexts : public ilanguagetext
    {
    public:
        
        //! Returns the translation for @a text. If no translation for @a text
        //! is found, the original text is returned.
        //! The empty text always returns the empty text.
        utf8text text_for( const utf8text& text ) const override;

        //! Assigns a translation for @a original to @a meaning. @a original should be 
        //! the source text, usually located in the binary or data, and @a meaning
        //! should be the localized text in the language this object represents.
        //! If original is empty but meaning is not empty, this will throw.
        //! The empty text is only permitted to translate to another empty text.
        void add_translation( const utf8text& original, const utf8text& meaning );

    private:
        
        std::map<utf8text, utf8text> texts_;
    };
}

#endif
