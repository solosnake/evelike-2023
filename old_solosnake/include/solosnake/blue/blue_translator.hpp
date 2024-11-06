#ifndef blue_translator_hpp
#define blue_translator_hpp

#include <memory>
#include <string>
#include "solosnake/ilanguagetext.hpp"

namespace blue
{
    class datapaths;
    class user_settings;

    //! This class ensures that the user settings are kept in synch with
    //! language changes (although it is possible for the setting to be
    //! changed without going through this object, it is not possible
    //! to change the application language without recording this in the
    //! user settings.
    //! The default constructed translator will try to load whatever language
    //! is set in the settings, else will revert to the default settings
    //! language (none) which performs no translations on texts.
    class translator : public solosnake::ilanguagetext
    {
    public:

        explicit translator(
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<datapaths>& );

        //! Dtor exists so we can check we are not getting any cyclic smart ptr issues.
        ~translator();

        //! Attempts to find a language file matching the named file
        //! and if found loads the associated translations and returns
        //! true. Returns false if no matching file could be found or
        //! if the load failed somehow. Records the language change
        //! in the persistance object.
        //! If the newlanguagefilename is the empty string, then
        //! the translator will stop translating texts and operate
        //! as a "pass through" translator, and the resulting 'translation'
        //! for any given text will just be itself.
        bool change_language( const std::string& newlanguagefilename );

        //! Returns the translation for the given text in the current language.
        solosnake::utf8text text_for( const solosnake::utf8text& ) const override; // final.

        //! Returns the language path for the currently set language. This
        //! can be the empty path if there is no translation active.
        std::string current_language_filename() const;

    private:

        std::shared_ptr<user_settings>              settings_;
        std::shared_ptr<solosnake::ilanguagetext>   language_;
        std::shared_ptr<datapaths>                  paths_;
    };
}

#endif
