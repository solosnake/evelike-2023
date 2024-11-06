#ifndef blue_user_settings_hpp
#define blue_user_settings_hpp

#include <algorithm>
#include <iosfwd>
#include <string>
#include <memory>
#include "solosnake/persistance.hpp"

namespace blue
{
    //! Everything is read (and written to) the settings file: app data dir etc.
    class user_settings : public solosnake::persistance
    {
    public:

        //! @param settingsFile  File to load to populate this settings object.
        //! @param userSaveDir   Directory user files should be saved to.
        //! @param gameDataDirs  List of directories used to create a datapaths
        //!                      with. When not listed in the user settings
        //!                      file, this value is used instead.
        user_settings(
            const std::string& settingsFile,
            const std::string& userSaveDir,
            const std::string& gameDataDirs );

        virtual ~user_settings();

        void save() const;

        const std::string data_dirs() const;

    private:

        void set_required_keys( const std::string&, const std::string& );

        void add_settings();

        void reset_to_safe_defaults();

        void print_to( std::ostream& ) const;

        void always_set( const std::string&, const std::string&, const std::string& );

    private:

        std::string src_filename_;
    };
}

#endif
