#ifndef blue_log_events_hpp
#define blue_log_events_hpp

#include <string>

namespace blue
{
    class Machine;
    class Machine_event;

    void log_event( const Machine_event& );

    void log_machine_event( const Machine_event& );

    void cout_machine( const Machine&, const std::string& );

    void log_machine( const Machine&, const std::string& );

    void err_machine( const Machine&, const std::string& );

}

#endif
