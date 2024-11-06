#include <cstdio>
#include "solosnake/blue/blue_log_events.hpp"
#include "solosnake/blue/blue_machine.hpp"
#include "solosnake/blue/blue_machine_event.hpp"
#include "solosnake/logging.hpp"

namespace blue
{
    namespace
    {
        std::string event_to_str( const Machine_event& e )
        {
            switch( e.event_id() )
            {
                case EMachineAddedToBoard:
                    return std::string( "EMachineAddedToBoard: " )
                           + std::string( e.machine_->name() )
                           + " added to board.";

                case EMachinePositionalInfoUpdate:
                    return std::string( "EMachinePositionalInfoUpdate: " )
                           + std::string( e.machine_->name() )
                           + " changed position.";
            }

            return "ERROR OR MISSING EVENT";
        }
    }

#ifdef SS_COMPILE_WITHOUT_LOGGING

    void log_machine_event( const Machine_event& )
    {
    }

    void log_event( const Machine_event& )
    {
    }

    void log_machine( const Machine&, const std::string& )
    {
    }

    void err_machine( const Machine&, const std::string& )
    {
    }

#else

    void log_machine_event( const Machine_event& e )
    {
        std::string eventLog = std::string( event_to_str( e ) ) + std::string( " (from Machine)\n" );

        if( EMachinePositionalInfoUpdate == e.event_id() )
        {
            // Skip the position ones, too noisy.
        }
        else
        {
            ss_log( eventLog );
        }
    }

    void log_event( const Machine_event& e )
    {
        ss_log( event_to_str( e ) );
    }

    void log_machine( const Machine& m, const std::string& s )
    {
        ss_log( m.name(), ":", s );
    }

    void err_machine( const Machine&, const std::string& s )
    {
        ss_err( s.c_str() );
    }

#endif

    void cout_machine( const Machine& m, const std::string& s )
    {
        std::cout << m.name() << ":" << s << "\n";
    }
}
