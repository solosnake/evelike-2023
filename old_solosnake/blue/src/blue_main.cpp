#include <iostream>
#include <chrono>
#include <iomanip>
#include <exception>
#include <sstream>
#include <thread>
#include "solosnake/blue/blue_application_paths.hpp"
#include "solosnake/blue/blue_get_buildstamp.hpp"
#include "solosnake/blue/blue_program_options.hpp"
#include "solosnake/blue/blue_main.hpp"
#include "solosnake/blue/blue_start.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/logging.hpp"

using namespace std::chrono_literals;

namespace
{
    std::string logfilename()
    {
        std::ostringstream ss;
        time_t rawtime;
        const struct tm* timeinfo;
        time( &rawtime );
        timeinfo = localtime( &rawtime );

        ss << "log"
           << ( 1900 + timeinfo->tm_year )
           << std::setw( 2 )
           << std::setfill( '0' )
           << ( 1 + timeinfo->tm_mon )
           << std::setw( 2 )
           << std::setfill( '0' )
           << timeinfo->tm_mday
           << std::setw( 2 )
           << std::setfill( '0' )
           << timeinfo->tm_hour
           << std::setw( 2 )
           << std::setfill( '0' )
           << timeinfo->tm_min
           << std::setw( 2 )
           << std::setfill( '0' )
           << timeinfo->tm_sec
           << ".txt";

        // WTF IS GOING ON?

        return ss.str();
    }
}

// Entry point for blue application.
int blue_main( const int argc, char const *const *const argv )
{
    using namespace blue;

    int result = 0;

    try
    {
        const blue_program_options options( argc, argv );

        // Set logging level.
        solosnake::g_ssLogLvl = options.logging_level();
        
        // By default log to file unless asked to log to cout.
        if( !options.log_to_cout() )
        {
            const std::string logpath( options.user_save_dir() + logfilename() );
            solosnake::open_log( logpath.c_str() );
        }
        
        if( options.run_blue() )
        {                     
            // Log the arguments.
            ss_log( "argv:" );
            for( int i = 0; i < argc; ++i )
            {
                ss_log( argv[i] );
            }

            auto userSettings = std::make_shared<user_settings>( options.settings_file(),
                                                                 options.user_save_dir(),
                                                                 options.game_data_dirs() );

            result = start( userSettings );
        }
        else
        {
            // We ignore the options to log to file and always log these to cout (default)
            if( options.show_version() )
            {
                std::cout << options.version() << std::endl;
            }

            if( options.show_help() )
            {
                std::cout << options.description() << std::endl;
            }
        }
    }
    catch( const std::exception& e )
    {
        ss_err( "Exception : ", e.what() );
        // Try to ALWAYS output some data.
        std::cerr << "Exception, " << e.what();
        result = -3;
    }
    catch( ... )
    {
        ss_err( "Unknown exception." );
        // Try to ALWAYS output some data.
        std::cerr << "Exception, unknown.";
        result = -4;
    }

    if( result != 0 )
    {
        ss_err( BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME " encountered a problem.\n" );
        std::this_thread::sleep_for( 10s );
    }

    solosnake::close_log();
    return result;
}
