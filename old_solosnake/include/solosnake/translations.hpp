#ifndef solosnake_translations_hpp
#define solosnake_translations_hpp

#include "solosnake/ilanguagetext.hpp"

namespace solosnake
{
    class manifest;
    class ilanguagetext;

    //! Returns true if the manifest has a translation that matches @a translationname.
    bool is_translation_available( const manifest& translations, const char* translationname );

    //! Tries to load and return the translation xml file at @a filepath. Returns the empty
    //! pointer if no matching translation could be found or loaded.
    std::unique_ptr<ilanguagetext> load_translation( const char* filepath );
}

#endif 
