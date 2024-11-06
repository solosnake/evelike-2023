#ifndef solosnake_screenloop_hpp
#define solosnake_screenloop_hpp

#include <memory>
#include <string>
#include <vector>
#include "solosnake/dimension.hpp"
#include "solosnake/fullscreen.hpp"
#include "solosnake/igameloop.hpp"
#include "solosnake/minimised.hpp"
#include "solosnake/observer.hpp"
#include "solosnake/window.hpp"
#include "solosnake/windowchange.hpp"

namespace solosnake
{
    class iinput_events;
    class Rect;
    class screenloophooks;

    //! Class to tie the window to a game loop.
    class screenloop
        : public igameloop
        , public observer<windowchange, window>
    {
    protected:

        struct HeapOnly { };

    public:

        static std::shared_ptr<screenloop> make_shared( const std::shared_ptr<window>& );

        screenloop( const std::shared_ptr<window>&, const HeapOnly& );

        virtual ~screenloop();

        void on_event( const windowchange&, const window& ) override;

        void render_screen( const unsigned long dt );

        void advance_game_one_frame() override
        {
        }

    protected:

        //! Override this to handle screen resized messages.
        //! By default this does nothing.
        virtual void on_window_resized( const dimension2d<unsigned int>& );

        //! Override this to handle screen full-screen changed messages.
        //! By default this does nothing.
        virtual void on_window_fullscreen_changed( FullscreenState );

        //! Override this to handle minimised messages.
        //! By default this does nothing.
        virtual void on_window_minimised_changed( MinimisedState );

        //! All events are assumed to be processed by this call. Each event will be passed to the
        //! update only once.
        virtual LoopResult update_loop( const iinput_events& );

    private:

        //! Called by the screenloop to start the scene. View matrices etc should be initialised
        //! here and the renderer scene started.
        //! By default this method does nothing.
        virtual void start_scene( const unsigned long ms );

        //! Called by the screenloop to render the scene. Here views and backgrounds and other
        //! scene contents should be rendered. The scene should not be started or ended by this
        //! callback.
        //! By default this method does nothing.
        virtual void render_scene( const unsigned long ms );

        //! Called by the screenloop to end the scene. Cleanup of objects build just for this loop
        //! update can be performed here. The renderer's scene should be ended here and the buffers
        //! swapped, and any other actions such as sound rendering etc can take place here.
        //! By default this method does nothing.
        virtual void end_scene( const unsigned long ms );

        //! Notification received from window message.
        void handle_fullscreen_changed( const FullscreenState&, const window& );

        //! Notification received from window message.
        void handle_minimised_changed( const MinimisedState&, const window& );

        //! Notification received from window message.
        void handle_screensize_changed( const dimension2d<unsigned int>&, const window& );

        void handle_dragdrops( const std::vector<std::wstring>&, const window& );

        void initial_preframe_update() override;

        LoopResult preframe_update( const unsigned long milliseconds ) override;

        //! This will always be called once.
        LoopResult initial_frame_update() override;

        //! Continuously called during loop with the delta since last
        //! 'frame_update' call in ms. Returning false will stop the loop.
        LoopResult frame_update( const unsigned long milliseconds ) override;

        //! The application is killing the update loop.
        void final_frame_update( const unsigned long milliseconds ) override;

        LoopResult do_one_screenloop( const unsigned long dt );

    private:

        screenloop( const screenloop& );
        screenloop& operator=( const screenloop& );

    private:

        std::shared_ptr<window>         window_;
    };
}

#endif
