#ifndef blue_iscreenview_hpp
#define blue_iscreenview_hpp

#include <string>
#include "solosnake/dimension.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/iinput_events.hpp"
#include "solosnake/loopresult.hpp"
#include "solosnake/minimised.hpp"
#include "solosnake/rect.hpp"

namespace blue
{
    //! The view is actually a 'view-controller'. It handles inputs, interprets
    //! them, and changes the shared model.
    class iscreenview
    {
    public:

        explicit iscreenview( const std::string& );

        virtual ~iscreenview();

        const std::string& screenview_name() const;

        //! @a dt The elapsed milliseconds since the last call to render_view. If dt is
        //! is zero the view should be rendered in the same state as the previous 
        //! rendering (if any).
        virtual void render_view( const unsigned long dt ) const = 0;

        virtual void handle_fullscreen_changed( const solosnake::FullscreenState& ) = 0;

        virtual void handle_minimised_changed( const solosnake::MinimisedState& ) = 0;

        virtual void handle_screensize_changed( const solosnake::dimension2d<unsigned int>& ) = 0;

        // Return true to continue looping.
        virtual solosnake::LoopResult handle_inputs( const solosnake::iinput_events& ) = 0;

        // Called when the controller becomes the active one.
        virtual void activate_view() = 0;

        // Called when the controller is about to be deactivated.
        virtual void deactivate_view() = 0;

        //! Allows view to do any processing that is per game frame.
        virtual void advance_one_frame() = 0;

    private:

        std::string screenview_name_;
    };

    //-------------------------------------------------------------------------

    inline const std::string& iscreenview::screenview_name() const
    {
        return screenview_name_;
    }
}

#endif
