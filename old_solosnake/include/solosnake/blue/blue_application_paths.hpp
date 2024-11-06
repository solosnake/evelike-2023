#ifndef blue_application_paths_hpp
#define blue_application_paths_hpp

#include <string>

namespace blue
{
    //! When no settings file is specified in the startup, try to find it.
    //! e.g.  C:\Users\solosnake\AppData\Roaming\solosnake\blue\blue-settings.xml
    //! e.g.  /home/solosnake/local/bin/blue/blue-settings.xml
    std::string get_user_settings_filepath();

    //! Attempts to find a writable user data path.
    std::string get_userdata_path();

    //! Attempts to return the application directory.
    std::string get_game_data_dir();
}

#endif
