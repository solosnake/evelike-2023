#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    std::ostream* g_clog = &std::cout;
    std::ostream* g_cerr = &std::cerr;
    unsigned int g_ssLogLvl = SS_LOG_WRN | SS_LOG_ERR;

    namespace
    {
      std::unique_ptr<std::ostream> g_clogfile;
    }


#ifndef SS_COMPILE_WITHOUT_LOGGING

    namespace
    {
        void format_logging( std::ostream* s )
        {
            if( s )
            {
                *s << std::setprecision( SS_LOG_PRECISION );
#if SS_LOG_FIXEDWIDTH
                *s << std::fixed;
#else
                s->setf( std::ios_base::floatfield );
#endif
            }
        }

        /// Forces formatting of logging streams.
        struct log_formatting
        {
            log_formatting()
            {
                format_logging( g_clog );
                format_logging( g_cerr );
            }
        };

        log_formatting g_logformatting;
    }

    void close_log()
    {
        if( g_clogfile )
        {
            g_clog->flush();
            g_cerr->flush();
            g_clog = &std::cout;
            g_cerr = &std::cerr;
            g_clogfile.reset();
        }
    }

    std::ostream* open_log( const char* filename )
    {
        close_log();

        std::ostream* log = nullptr;

        g_clogfile.reset( new std::ofstream( std::string( filename ) ) );

        if( g_clogfile->good() )
        {
            format_logging( g_clogfile.get() );

            // All logs are output to this file.
            g_clog = g_cerr = log = g_clogfile.get();
        }
        else
        {
            ss_throw( "Unable to create logfile." );
        }

        return log;
    }

#endif
} // blue
