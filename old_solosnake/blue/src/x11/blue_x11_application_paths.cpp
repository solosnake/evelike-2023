#include "solosnake/blue/blue_application_paths.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/logging.hpp"
#include <filesystem>


namespace blue
{
    std::string get_user_settings_filepath()
    {
        return std::filesystem::current_path() / AO_USER_SETTINGS_FILENAME_VALUE;
    }

    std::string get_userdata_path()
    {
        return std::filesystem::current_path().string();
    }

    std::string get_game_data_dir()
    {   
        ss_wrn("THE GAME DATA DIR IS SET TO CWD.");
        return std::filesystem::current_path().string();
    }
}
