#ifndef solosnake_rendering_system_hpp
#define solosnake_rendering_system_hpp

#include <memory>
#include "solosnake/dimension.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/ibackbuffers.hpp"
#include "solosnake/observer.hpp"
#include "solosnake/renderer_cache.hpp"
#include "solosnake/window.hpp"

namespace solosnake
{
    class ifilefinder;

    //! Intended to a be a one-stop object typing all the various
    //! requirements together.
    //! Implements the deferred_renderer and also ties the
    //! deferred_renderer to the rendering window.
    class rendering_system : public ibackbuffers
    {
        struct HeapOnly
        {
        };

    public:

        static std::shared_ptr<rendering_system> create(
            std::shared_ptr<window>,
            deferred_renderer::Quality,
            std::shared_ptr<ifilefinder> modelspaths,
            std::shared_ptr<ifilefinder> fontspaths,
            std::shared_ptr<imesh_cache>,
            std::shared_ptr<iimg_cache>,
            std::shared_ptr<iskybox_cache>);

        rendering_system(
            std::shared_ptr<window>,
            deferred_renderer::Quality,
            std::shared_ptr<ifilefinder> modelspaths,
            std::shared_ptr<ifilefinder> fontspaths,
            std::shared_ptr<imesh_cache>,
            std::shared_ptr<iimg_cache>,
            std::shared_ptr<iskybox_cache>,
            const HeapOnly& );

        virtual ~rendering_system();

        void swap_buffers() override;

        Rect get_window_rect() const;

        Rect get_screen_rect() const;

        dimension2d<unsigned int> get_window_dimensions() const;

        std::shared_ptr<window> get_rendering_window_ptr();

        std::shared_ptr<renderer_cache> get_cache_ptr();

        std::shared_ptr<deferred_renderer> get_renderer_ptr();

        renderer_cache& cache();

        deferred_renderer& renderer();

    private:

        void handle_screensize_changed( const dimension2d<unsigned int>&, const window& );

        std::shared_ptr<callback<windowchange, window>>     window_events_;
        std::shared_ptr<window>                             window_;
        std::shared_ptr<renderer_cache>                     renderer_;
        Rect                                                window_rect_cached_;
        Rect                                                screen_rect_cached_;
        dimension2d<unsigned int>                           window_dimensions_cached_;
    };

    //-------------------------------------------------------------------------

    inline std::shared_ptr<window> rendering_system::get_rendering_window_ptr()
    {
        return window_;
    }

    inline std::shared_ptr<renderer_cache> rendering_system::get_cache_ptr()
    {
        return renderer_;
    }

    inline std::shared_ptr<deferred_renderer> rendering_system::get_renderer_ptr()
    {
        return ( *renderer_ ).get_renderer_ptr();
    }

    inline renderer_cache& rendering_system::cache()
    {
        return *renderer_;
    }

    inline deferred_renderer& rendering_system::renderer()
    {
        return ( *renderer_ ).get_renderer();
    }

    inline Rect rendering_system::get_window_rect() const
    {
        return window_rect_cached_;
    }

    inline Rect rendering_system::get_screen_rect() const
    {
        return screen_rect_cached_;
    }

    inline dimension2d<unsigned int> rendering_system::get_window_dimensions() const
    {
        return window_dimensions_cached_;
    }
}

#endif
