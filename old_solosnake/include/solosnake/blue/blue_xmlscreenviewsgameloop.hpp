#ifndef blue_xmlscreensviewsgameloop_hpp
#define blue_xmlscreensviewsgameloop_hpp

#include "solosnake/dimension.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/iinput_events.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/sound_renderer_fwd.hpp"
#include "solosnake/screenloop.hpp"

namespace blue
{
    class widgetsfactory;
    class xmlscreenviews;

    //! A class that allows an xml screens view to be used as a game loop (via
    //! the screenloop class, which is a windowed game loop).
    class xmlscreensviewsgameloop : public  solosnake::screenloop
    {
    public:

        xmlscreensviewsgameloop(
            const std::shared_ptr<solosnake::window>&,
            const std::shared_ptr<xmlscreenviews>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::screenloop::HeapOnly& );

        virtual ~xmlscreensviewsgameloop();

        //! Reads the XML @a xmlScreenFile and creates the screen's views, then
        //! uses these to construct a game loop.
        static std::shared_ptr<xmlscreensviewsgameloop> make_shared(
            const solosnake::filepath& xmlScreenFile,
            const std::shared_ptr<solosnake::window>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<widgetsfactory>& );

        static std::shared_ptr<xmlscreensviewsgameloop> make_shared(
            const std::shared_ptr<solosnake::window>&,
            const std::shared_ptr<xmlscreenviews>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>& );

    protected:
        
        //! When inheriting from this class, it is important to remember that it needs
        //! to observe changes in the window to behave correctly (resizes etc).
        static void add_as_observer_of_window( 
            const std::shared_ptr<xmlscreensviewsgameloop>&,
            const std::shared_ptr<solosnake::window>&);

        //! Starts the renderer scene.
        void start_scene( const unsigned long ms ) override;

        //! Renders the views.
        void render_scene( const unsigned long ms ) override;

        //! Ends the renderer scene and updates the sounds and swaps the renderer buffers.
        void end_scene( const unsigned long ms ) override;

        void advance_game_one_frame() override
        {
        }

    private:

        void on_window_resized( const solosnake::dimension2d<unsigned int>& ) override;

        void on_window_fullscreen_changed( solosnake::FullscreenState ) override;

        void on_window_minimised_changed( solosnake::MinimisedState ) override;

        solosnake::LoopResult update_loop( const solosnake::iinput_events& ) override;

    private:

        std::shared_ptr<xmlscreenviews>                 views_;
        std::shared_ptr<solosnake::rendering_system>    rendering_;
        std::shared_ptr<solosnake::sound_renderer>      sounds_;
    };
}

#endif
