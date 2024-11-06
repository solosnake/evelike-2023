#ifndef solosnake_windowinputs_hpp
#define solosnake_windowinputs_hpp
#include <memory>
#include "solosnake/throw.hpp"
#include "solosnake/inputs.hpp"
#include "solosnake/ioswindow.hpp"

namespace solosnake
{
    //! Class to create and expose iinput_events from a window.
    class windowinputs : public iinput_events
    {
    public:

        explicit windowinputs( std::shared_ptr<ioswindow> w ) : window_( w )
        {
            if( !window_ )
            {
                ss_throw( "Window pointer argument was null pointer." );
            }
        }

        std::shared_ptr<const ioswindow> window() const
        {
            return window_;
        }

        std::shared_ptr<ioswindow> window()
        {
            return window_;
        }

        const std::vector<input_event>& events() const override
        {
            return window()->input_changes().events();
        }

        void clear_events() override
        {
            return window()->clear_input_events();
        }

        bool is_shutdown() const override
        {
            return window()->is_shutdown();
        }

    private:

        std::shared_ptr<ioswindow> window_;
    };

    typedef std::shared_ptr<windowinputs> windowinputs_ptr;
}

#endif
