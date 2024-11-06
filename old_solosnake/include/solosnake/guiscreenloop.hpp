#if 0

#ifndef guiscreenloop_hpp
#define guiscreenloop_hpp

#include "solosnake/dimension.hpp"
#include "solosnake/screenloop.hpp"
#include "solosnake/ixml_widgetfactory.hpp"

namespace solosnake
{
    class gui;
    class window;
    class rendering_system;
    class sound_renderer;

    //! Screen loop for use with gui. Ties the screenloop, which is a window, 
    //! to the gui and so to the command executor.
    //! A gui screen loop is a complete "view", and is not a sub component. It
    //! is used when you want to have a view that is completely gui driven. This
    //! should not be confused with gui components. This is a screenloop that
    //! uses a gui collection.
    class guiscreenloop : public screenloop
    {
    public:

        static std::shared_ptr<guiscreenloop> 
        make_shared( 
                const std::shared_ptr<gui>&,
                const std::shared_ptr<window>&,
                const std::shared_ptr<rendering_system>&,
                const std::shared_ptr<sound_renderer>& );

        std::shared_ptr<gui> get_gui() const
        {
            return gui_;
        }

        guiscreenloop( 
            const std::shared_ptr<gui>&,
            const std::shared_ptr<window>&,
            const std::shared_ptr<rendering_system>&,
            const std::shared_ptr<sound_renderer>&,
            const screenloop::HeapOnly& );

    private:

        void render_loop( const unsigned long dt ) override;

        LoopResult update_loop( const iinput_events& ) override;

        void on_window_resized( const dimension2d<unsigned int>& ) override;

        void resize_screen( const dimension2d<unsigned int>& );

    private:

        std::shared_ptr<rendering_system> renderer_;
        std::shared_ptr<sound_renderer>   sounds_;
        std::shared_ptr<gui>              gui_;
    };
}

#endif

#endif
