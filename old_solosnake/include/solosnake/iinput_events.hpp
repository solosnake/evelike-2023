#ifndef solosnake_iinput_events_hpp
#define solosnake_iinput_events_hpp
#include <vector>
#include "solosnake/input_event.hpp"

namespace solosnake
{
    //! Abstract interface to a provider of a  vector of generic input events,
    //! sorted by their time of arrival (earliest first). This is processed and
    //! then cleared by the gui.
    class iinput_events
    {
    public:

        virtual ~iinput_events();

        virtual const std::vector<input_event>& events() const = 0;

        virtual void clear_events() = 0;

        //! If this returns true the underlying window is closed.
        virtual bool is_shutdown() const = 0;
    };
}

#endif
