#ifndef blue_xmlscreenviews_hpp
#define blue_xmlscreenviews_hpp

#include <memory>
#include <vector>
#include "solosnake/filepath.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/iscreenstate.hpp"
#include "solosnake/iinput_events.hpp"
#include "solosnake/loopresult.hpp"
#include "solosnake/minimised.hpp"
#include "solosnake/nextscreen.hpp"
#include "solosnake/rect.hpp"

namespace blue
{
    class iscreenview;
    class widgetsfactory;

    //! Within the blue application, a 'screen' is an implementation of the
    //! solosnake::iscreen concept: a mini standalone 'app' within the
    //! application, representing perhaps a 'page' or a 'game'. A screen
    //! is created, and control passes to it, and remains there until the
    //! screen is finished, when it returns the name of the next screen to show.
    //!
    //! In the blue application the screens are data driven, and are typed.
    //! The type of the screen decides what is shown on the screen and what
    //! is exposed to the Lua instance associated with the screen. Each screen
    //! is read from an XML file which contains a 'screen' element which has a
    //! 'type' attribute.
    //!
    //! A screen uses an instance of an xmlscreenviews class to load the views
    //! from xml, and to form a bridge, via a shared Lua instance, with the
    //! screen which owns the xmlscreenviews instance.
    //!
    //! As the widgets and their Lua contain functions which can reference this
    //! object, the xmlscreensviews object must not 'leak' its widget
    //! pointers to the public. It owns and maintains their lifetime.
    class xmlscreenviews : public std::enable_shared_from_this<xmlscreenviews>
    {
        struct HeapOnly
        {
        };

    public:

        explicit xmlscreenviews( const HeapOnly& );

        static std::shared_ptr<xmlscreenviews> make_shared(
            const std::shared_ptr<iscreenview>&,
            const std::shared_ptr<solosnake::iscreenstate>& );

        //! Create a set of views from an XML screen document.
        static std::shared_ptr<xmlscreenviews> make_shared(
            const solosnake::filepath&,
            const solosnake::dimension2d<unsigned int>&,
            const std::shared_ptr<widgetsfactory>& );

        bool try_set_as_active_view( const char* );

        void advance_one_frame();

        //! @a dt is the elapsed milliseconds since the previous call
        //! to render_active_view (if any).
        void render_active_view( const unsigned long dt ) const;

        void handle_fullscreen_changed( const solosnake::FullscreenState& );

        void handle_minimised_changed( const solosnake::MinimisedState& );

        void handle_screensize_changed( const solosnake::dimension2d<unsigned int>& );

        solosnake::LoopResult handle_inputs( const solosnake::iinput_events& );

        solosnake::nextscreen get_next_screen() const;

    private:

        void store_view( const std::shared_ptr<iscreenview>& );

        void set_screensize( const solosnake::dimension2d<unsigned int>& );

        void set_screen_state( const std::shared_ptr<solosnake::iscreenstate>& );

        void set_as_active_view( const std::shared_ptr<iscreenview>& );

        std::shared_ptr<iscreenview> try_get_named_view( const char* ) const;

        std::shared_ptr<iscreenview> try_get_named_view( const std::string& ) const;

    private:

        std::vector<std::shared_ptr<iscreenview>>   views_;
        std::shared_ptr<iscreenview>                active_view_;
        std::shared_ptr<solosnake::iscreenstate>    screen_state_;
    };

}

#endif
