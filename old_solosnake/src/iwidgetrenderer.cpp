#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/deferred_renderer.hpp"

namespace solosnake
{
    iwidgetrenderer::iwidgetrenderer(
        const std::string& style,
        const std::shared_ptr<rendering_system>& rendering )
        : renderer_( &rendering->renderer() )
        , rendering_( rendering )
        , stylename_( style )
    {
    }

    iwidgetrenderer::~iwidgetrenderer()
    {
    }

    quad iwidgetrenderer::screencoord_quad( const rect& area ) const
    {
        return iwidgetrenderer::area_to_screencoord_quad(
            area,
            rendering_->get_window_dimensions() );
    }

    //! Returns a screen coordinate quad of a pixel based area.
    quad iwidgetrenderer::area_to_screencoord_quad( const rect& r,
                                                    const dimension2d<unsigned int>& screenSize )
    {
        const float w = static_cast<float>( screenSize.width() );
        const float h = static_cast<float>( screenSize.height() );

        return quad( static_cast<float>( r.left() )   / w,
                     static_cast<float>( r.top() )    / h,
                     static_cast<float>( r.width() )  / w,
                     static_cast<float>( r.height() ) / h );
    }    
    
    std::shared_ptr<deferred_renderer> iwidgetrenderer::renderer_ptr()
    {
        return rendering_->get_renderer_ptr();
    }
}
