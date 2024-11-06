#include <iostream>
#include "solosnake/logging.hpp"
#include "solosnake/show_console.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_program_options.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_application_paths.hpp"
#include "solosnake/blue/blue_get_buildstamp.hpp"

namespace blue
{
    blue_program_options::blue_program_options( const int argc, char const *const *const argv )
        : settingsFile_("settings.xml")
        , saveDir_(".")
        , dataDirs_(".")
        , desc_()
        , version_()
        , loggingLevel_( SS_DEFAULT_LOGGINGLEVEL )
        , log_to_cout_( true )
        , run_( true )
        , show_help_( false )
        , show_version_( false )
    {
        using namespace std;

        //std::cerr << __FILE__ << " IS NOT IMPLEMENTED.";

        settingsFile_ = get_user_settings_filepath();
        saveDir_ = get_userdata_path();
        dataDirs_ = get_game_data_dir();

        if (log_to_cout_)
        {
            solosnake::show_console();
        }

        if (show_version_)
        {
            run_ = false;
            version_ = get_buildstamp();
        }

        /*
        
        // Declare the supported options.
        boost::program_options::options_description desc(
            BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME " options" );

        desc.add_options()
        ( "help,h", "Show help message" )
        ( "version,v", "Show version" )
        ( "cout,c", "Redirect logging to cout" )
        ( "settings,u", value<string>(), "File to use as user settings" )
        ( "save-dir,s", value<string>(), "User game save(s) directory" )
        ( "data-dir,d", value<string>(), "Game static data directory" )
        ( "logging,l",  value<unsigned int>( &loggingLevel_ )->default_value( SS_DEFAULT_LOGGINGLEVEL ), 
                                                                              "0 = None\n1 = Errors\n"
                                                                              "2 = Errors & Warnings\n"
                                                                              "3 = Errors & Warnings & Logs" )
        ;

        variables_map vm;
        store( parse_command_line( argc, argv, desc ), vm );
        notify( vm );

        show_help_    = vm.count( "help" )    > 0u;
        show_version_ = vm.count( "version" ) > 0u;
        log_to_cout_  = vm.count( "cout" )    > 0u;

        if( log_to_cout_ )
        {
            solosnake::show_console();
        }

        if( show_help_ )
        {
            std::ostringstream os;
            os << desc;
            run_  = false;
            desc_ = os.str();
        }

        if( show_version_ )
        {
            run_     = false;
            version_ = get_buildstamp();
        }

        if( vm.count( "settings" ) )
        {
            settingsFile_ = vm["settings"].as<std::string>();
        }
        else
        {
            settingsFile_ = get_user_settings_filepath();
        }

        if( vm.count( "save-dir" ) )
        {
            saveDir_ = vm["save-dir"].as<std::string>();
        }
        else
        {
            saveDir_ = get_userdata_path();
        }

        if( vm.count( "data-dir" ) )
        {
            dataDirs_ = vm["data-dir"].as<std::string>();
        }
        else
        {
            dataDirs_ = get_game_data_dir();
        }

        */

        if( loggingLevel_ > SS_LOG_ALL )
        {
            ss_throw( "Invalid logging option" );
        }
    }

    const std::string& blue_program_options::settings_file() const
    {
        return settingsFile_;
    }

    const std::string& blue_program_options::user_save_dir() const
    {
        return saveDir_;
    }

    const std::string& blue_program_options::game_data_dirs() const
    {
        return dataDirs_;
    }

    bool blue_program_options::log_to_cout() const
    {
        return log_to_cout_;
    }

    unsigned int blue_program_options::logging_level() const
    {
        return loggingLevel_;
    }
}
