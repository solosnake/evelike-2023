#ifndef solosnake_styledwidgetrenderer_hpp
#define solosnake_styledwidgetrenderer_hpp

#include <memory>
#include <utility>
#include <vector>
#include "solosnake/alignment.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/observer.hpp"
#include "solosnake/rendering_system_fwd.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/windowchange.hpp"
#include "solosnake/window.hpp"

class TiXmlElement;

namespace solosnake
{
    class window;
    class font;
    class fontfamily;
    class fontprinter;
    class ifilefinder;

    //! Holds on to a font, which holds on to a renderer. Thus the window
    //! to which the renderer refers to should remain alive during the lifetime
    //! of these objects (or you will get an OpenGL error during shutdown).
    //! Loads style information from XML file and renders widgets accordingly.
    //! Renders flat coloured buttons with a highlighting texture overlay.
    //! @code
    //! <style>
    //! <font size="0" file="Arial16.xml"/>
    //! <font size="1" file="Arial20.xml"/>
    //! <font size="-1" file="Arial4.xml"/>
    //! <buttons>
    //!     <normalup textbgra="" >
    //!         <base texture="MyGui.bmp" x0 y0 x1 y1 bgra0 bgra1 bgra2 bgra3 />
    //!         <overlay texture="MyGui.bmp" x0 y0 x1 y1 Bgra />
    //!     </normalup>
    //!     <normaldown textbgra="" >
    //!        <base texture="MyGui.bmp" x0 y0 x1 y1 bgra0 bgra1 bgra2 bgra3 />
    //!         <overlay texture="MyGui.bmp"/>
    //!     </normaldown>
    //!     <highlighted textbgra="" >
    //!     </highlighted>
    //!     <disabled textbgra="" >
    //!     </disabled>
    //! </buttons>
    //! </style>
    //! @endcode
    class styledwidgetrenderer
        : public iwidgetrenderer
        , public observer<windowchange, window>
    {
        struct HeapOnly
        {
        };

    public:

        static std::shared_ptr<styledwidgetrenderer> make_styledwidgetrenderer(
            const filepath& xmlFile,
            const std::shared_ptr<rendering_system>& );

        styledwidgetrenderer(
            const filepath& xmlFile,
            const std::shared_ptr<rendering_system>&,
            const HeapOnly& );

        virtual ~styledwidgetrenderer();

        void render_button_back(
            const quad&,
            WidgetVisualStates ) override;

        void render_button_highlight(
            const quad&,
            WidgetVisualStates ) override;

        void render_checkbox(
            const quad&,
            WidgetVisualStates,
            const bool checked ) override;

        std::pair<position2df,position2df> render_text(
            const utf8text&,
            const int relsize,
            const position2df& screenpos,
            const dimension2df& size,
            HorizontalAlignment,
            VerticalAlignment,
            WidgetVisualStates ) override;

        void on_event(
            const windowchange&,
            const window& ) override;

        struct tex_and_quad
        {
            tex_and_quad();

            void load( const TiXmlElement&, rendering_system& );

            void render( const quad& dest, deferred_renderer& r ) const;

            texturehandle_t                 texhandle_;
            deferred_renderer::screen_quad  quad_;
        };

    private:

        struct buttonstyle_layer
        {
            buttonstyle_layer();

            void render( const quad&, deferred_renderer& ) const;

            void load( const TiXmlElement*, rendering_system& );

            tex_and_quad    layer_;
        };

        struct buttonstyle
        {
            buttonstyle();

            static void load_styles(
                buttonstyle* const buttonStyles,
                TiXmlElement* const pButtonsElement,
                const std::shared_ptr<rendering_system>& rr );

            void load( TiXmlElement&, rendering_system& );

            Bgra                            fontColour_;
            buttonstyle_layer               background_;
            buttonstyle_layer               highlight_;
        };

        struct checkbox
        {
            checkbox();

            void load( TiXmlElement&, rendering_system& );

            void render( const quad&, const bool checked, deferred_renderer& ) const;

            tex_and_quad    checked_;
            tex_and_quad    unchecked_;
        };

    private:

        Bgra get_font_colour(
            WidgetVisualStates ) const;

        void set_screensize(
            const Dimension2d<unsigned int>& );

        void handle_screensize_changed(
            const Dimension2d<unsigned int>& newsize,
            const window& );

    private:

        std::shared_ptr<fontprinter>    fontprinter_;
        std::shared_ptr<fontfamily>     fontfamily_;
        checkbox                        checkbox_;
        buttonstyle                     button_styles_[SOLOSNAKE_WIDGET_VISUALSTATES_COUNT];
    };

    //-------------------------------------------------------------------------

    inline Bgra styledwidgetrenderer::get_font_colour( WidgetVisualStates bs ) const
    {
        return button_styles_[bs].fontColour_;
    }
}

#endif
