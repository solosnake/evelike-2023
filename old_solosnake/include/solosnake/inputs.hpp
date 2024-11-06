#ifndef solosnake_iinputs_hpp
#define solosnake_iinputs_hpp
#include <vector>
#include "solosnake/input_event.hpp"
#include "solosnake/iinput_events.hpp"

namespace solosnake
{
    //! Interface to all the input options.
    class inputs : public iinput_events
    {
    public:

        inputs();

        const std::vector<input_event>& events() const override;

        void clear_events() override;

        bool is_shutdown() const override;

        void add_event( const input_event& );

        void shutdown();

        bool empty() const;

    private:

        std::vector<input_event>    events_;
        bool                        is_shutdown_;
    };

#include "solosnake/inputs.inl"
}

#endif
