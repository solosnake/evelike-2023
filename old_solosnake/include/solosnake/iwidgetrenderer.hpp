#ifndef solosnake_widgetrenderer_hpp
#define solosnake_widgetrenderer_hpp

#include <string>
#include <memory>
#include <utility>
#include "solosnake/alignment.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/position.hpp"
#include "solosnake/quad.hpp"
#include "solosnake/widgetvisualstates.hpp"

namespace solosnake
{
    class deferred_renderer;
    class rendering_system;
    class utf8text;
    class Rect;

    //! Provides a set of 'GUI' drawing functions for building GUI controls with.
    //! Each widget which renders can be passed an abstract widget renderer
    //! interface which knows how to draw given types in a certain style.
    class iwidgetrenderer
    {
    public:

        iwidgetrenderer(
            const std::string& style,
            const std::shared_ptr<rendering_system>& );

        virtual ~iwidgetrenderer();

        const std::string& stylename() const;

        const rendering_system& renderingsystem() const;

        rendering_system& renderingsystem();

        std::shared_ptr<rendering_system> renderingsystem_ptr();

        deferred_renderer& renderer();

        std::shared_ptr<deferred_renderer> renderer_ptr();

        quad screencoord_quad( const Rect& area ) const;

        virtual void render_checkbox(
            const quad&,
            WidgetVisualStates,
            const bool checked ) = 0;

        virtual void render_button_back(
            const quad&,
            WidgetVisualStates ) = 0;

        virtual void render_button_highlight(
            const quad&,
            WidgetVisualStates ) = 0;

        //! @param relSize The relative 'size' of the text. Default size
        //!                is zero, -1 is smaller, +1 is bigger etc
        virtual std::pair<position2df,position2df> render_text(
            const utf8text&,
            const int relSize,
            const position2df& screenpos,
            const dimension2df& size,
            HorizontalAlignment,
            VerticalAlignment,
            WidgetVisualStates ) = 0;

        static quad area_to_screencoord_quad(
            const Rect&,
            const Dimension2d<unsigned int>& screenSize );

    private:

        iwidgetrenderer( const iwidgetrenderer& );
        iwidgetrenderer& operator=( const iwidgetrenderer& );

    private:

        deferred_renderer*                  renderer_;
        std::shared_ptr<rendering_system>   rendering_;
        std::string                         stylename_;
    };
}

#include "solosnake/iwidgetrenderer.inl"

#endif
