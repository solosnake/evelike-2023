#ifndef blue_guiscreenview_hpp
#define blue_guiscreenview_hpp

#include <string>
#include "solosnake/gui.hpp"
#include "solosnake/iinput_events.hpp"
#include "solosnake/blue/blue_iscreenview.hpp"

namespace blue
{
    //! Reads in named view of screen and creates a gui within it.
    class guiscreenview : public iscreenview, private solosnake::iinput_events
    {
    public:

        guiscreenview(
            const std::shared_ptr<solosnake::gui>&,
            const std::string& );

        virtual ~guiscreenview();

        void render_view( const unsigned long dt ) const override;

        void handle_fullscreen_changed( const solosnake::FullscreenState& ) override;

        void handle_minimised_changed( const solosnake::MinimisedState& ) override;

        void handle_screensize_changed( const solosnake::dimension2d<unsigned int>& ) override;

        solosnake::LoopResult handle_inputs( const solosnake::iinput_events& ) override;

        void activate_view() override;

        void deactivate_view() override;

        void advance_one_frame() override;

    private:

        const std::vector<solosnake::input_event>& events() const override;

        void clear_events() override;

        bool is_shutdown() const override;

    private:

        std::vector<solosnake::input_event> fakeEvents_;
        std::shared_ptr<solosnake::gui>     gui_;
    };
}

#endif
