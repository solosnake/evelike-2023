#include "solosnake/filepath.hpp"
#include "solosnake/font.hpp"
#include "solosnake/fontfamily.hpp"
#include "solosnake/fontprinter.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/renderer_cache.hpp"
#include "solosnake/styledwidgetrenderer.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/external/xml.hpp"

using namespace std;

#define SOLOSNAKE_BASE_ELEMENT          "base"
#define SOLOSNAKE_BGRA_ELEMENT          "bgra"
#define SOLOSNAKE_BGRA0_ELEMENT         "bgra0"
#define SOLOSNAKE_BGRA1_ELEMENT         "bgra1"
#define SOLOSNAKE_BGRA2_ELEMENT         "bgra2"
#define SOLOSNAKE_BGRA3_ELEMENT         "bgra3"
#define SOLOSNAKE_BUTTONS_ELEMENT       "buttons"
#define SOLOSNAKE_CHECKBOXES_ELEMENT    "checkboxes"
#define SOLOSNAKE_CHECKED_ELEMENT       "checked"
#define SOLOSNAKE_UNCHECKED_ELEMENT     "unchecked"
#define SOLOSNAKE_FONT_ELEMENT          "font"
#define SOLOSNAKE_OVERLAY_ELEMENT       "overlay"
#define SOLOSNAKE_STYLE_ELEMENT         "style"
#define SOLOSNAKE_TEXTBGRA_ELEMENT      "textbgra"
#define SOLOSNAKE_TEXTURE_ELEMENT       "texture"
#define SOLOSNAKE_X0_ELEMENT            "x0"
#define SOLOSNAKE_X1_ELEMENT            "x1"
#define SOLOSNAKE_Y0_ELEMENT            "y0"
#define SOLOSNAKE_Y1_ELEMENT            "y1"

namespace solosnake
{
    namespace
    {
        static const bgra White = bgra( std::uint8_t( 0xFF ) );

        //! Strips the path from the filepath and returns just the file name for
        //! use as a style name. The stylename and filename are kept the same so
        //! we can use a style name to load a file without requiring some convention
        //! (e.g. stripping or adding a suffix) or a map of name <-> file.
        string stylename_from_file( const filepath& xmlfile )
        {
            return xmlfile.filename().string();
        }

        //! Reads and returns a font element:
        //! TODO: THIS CALL IS VERY SLOW!!
        fontfamily::sized_font_t load_font( const TiXmlElement& fontElement,
                                            renderer_cache& rcache )
        {
            int fontsize = 0;
            string fontfile;

            read_attribute( fontElement, "size", fontsize, false );
            read_attribute( fontElement, "file", fontfile, false );

            auto newFont = rcache.get_font( fontfile );

            return fontfamily::sized_font_t( newFont, fontsize );
        }

        //! Creates a family of fonts and returns it. Checks that this is not
        //! an empty family are done later.
        //! NOTE: This call is VERY SLOW!
        std::shared_ptr<fontfamily> load_fontfamily( const TiXmlNode& styleNode, renderer_cache& cache )
        {
            auto family = std::make_shared<fontfamily>();

            auto pFontElement = styleNode.FirstChildElement( SOLOSNAKE_FONT_ELEMENT );

            while( pFontElement )
            {
                family->add_sized_font( load_font( *pFontElement, cache ) );
                pFontElement = pFontElement->NextSiblingElement( SOLOSNAKE_FONT_ELEMENT );
            }

            return family;
        }
    }

    //-------------------------------------------------------------------------

    shared_ptr<styledwidgetrenderer>
    styledwidgetrenderer::make_styledwidgetrenderer( const filepath& xmlFile,
                                                     const shared_ptr<rendering_system>& rr )
    {
        auto styledrenderer = make_shared<styledwidgetrenderer>( xmlFile, rr, HeapOnly() );
        auto p = static_pointer_cast<observer<windowchange, window>>( styledrenderer );
        rr->get_rendering_window_ptr()->add_observer( p );
        return styledrenderer;
    }

    //
    // NOTE: This appears to be quite a slow function!!
    //
    styledwidgetrenderer::styledwidgetrenderer( const filepath& xmlFile,
                                                const shared_ptr<rendering_system>& rr,
                                                const HeapOnly& )
        : iwidgetrenderer( stylename_from_file( xmlFile ), rr )
    {
        auto t = make_timer();

        assert( filepath_exists( xmlFile ) );
        assert( rr );

        TiXmlDocument doc;
        load_xml_doc( doc, xmlFile.string().c_str() );
        ss_log( "Loaded XML\t\t\t", t->delta() );

        auto styleNode = doc.FirstChildElement( SOLOSNAKE_STYLE_ELEMENT );

        if( styleNode == nullptr )
        {
            ss_throw( "Missing '" SOLOSNAKE_STYLE_ELEMENT "' node." );
        }

        if( styleNode->NextSiblingElement( SOLOSNAKE_STYLE_ELEMENT ) != nullptr )
        {
            ss_throw( "Invalid XML\t\t" SOLOSNAKE_STYLE_ELEMENT
                      " document; only one style must be defined per document." );
        }

        // TODO: THIS CALL IS VERY SLOW!!!!
        fontfamily_ = load_fontfamily( *styleNode, rr->cache() );
        ss_log( "Loaded font sizes\t", t->delta() );

        // We insist on there being at least one font.
        if( fontfamily_->empty() )
        {
            ss_err( "No fonts defined in ", xmlFile.string() );
            ss_throw( "No fonts defined in XML widget rendering style file." );
        }
        else
        {
            // Find the font with size closest to zero, and use it as the
            // initial font.
            fontprinter_ = make_shared<fontprinter>( rr->get_window_dimensions(),
                                                     rr->get_renderer_ptr(),
                                                     fontfamily_->get_font_nearest_size( 0 ) );

            ss_log( "Made fontprinter\t", t->delta() );
        }

        // Find the buttons element, if any.
        if( auto pButtonsElement = styleNode->FirstChildElement( SOLOSNAKE_BUTTONS_ELEMENT ) )
        {
            buttonstyle::load_styles( button_styles_, pButtonsElement, rr );
            ss_log( "Loaded " SOLOSNAKE_BUTTONS_ELEMENT, t->delta() );
        }

        if( auto pCheckboxes = styleNode->FirstChildElement( SOLOSNAKE_CHECKBOXES_ELEMENT ) )
        {
            checkbox_.load( *pCheckboxes, *rr );
            ss_log( "Loaded " SOLOSNAKE_CHECKBOXES_ELEMENT, t->delta() );
        }

        set_screensize( rr->get_window_dimensions() );
    }

    styledwidgetrenderer::~styledwidgetrenderer()
    {
    }

    void styledwidgetrenderer::handle_screensize_changed( const dimension2d<unsigned int>& newsize,
                                                          const window& )
    {
        set_screensize( newsize );
    }

    // The fontprinter needs to be kept in synch with the screen size.
    void styledwidgetrenderer::set_screensize( const dimension2d<unsigned int>& size )
    {
        fontprinter_->set_screen_size( size );
    }

    //! Renders text within the given quad, using the font which corresponds
    //! to the given size. If a font has been registered with that size exactly,
    //! it will be used (even if it is different and/or with a different size).
    //! If no exact match is found, the registered font closest in size to this
    //! size is found and used, with the characters scaled accordingly. Each
    //! size step of 2 doubles or halves the printed character size.
    //! So the size can be both used to scale a set of fonts, and select between
    //! fonts.
    //!
    //! @param  relSize     The font used to draw with will be the font which
    //!                     closest matches this size. If no exact match is
    //!                     found, the closest sized font is used and scaled up
    //!                     or down towards the given size, with each size step
    //!                     of 2 corresponding to a doubling or halving of the
    //!                     printed character size.
    //!
    std::pair<position2df, position2df>
    styledwidgetrenderer::render_text( const utf8text& txt,
                                       const int relSize,
                                       const position2df& screenpos,
                                       const dimension2df& size,
                                       HorizontalAlignment halign,
                                       VerticalAlignment valign,
                                       WidgetVisualStates look )
    {
        auto fnt = fontfamily_->get_scaled_font( relSize );
        auto fntClr = get_font_colour( look );

        fontprinter_->set_font( fnt.first );
        fontprinter_->set_text_colour( fntClr );

        return fontprinter_->print_text( txt, screenpos, size, halign, valign, fnt.second );
    }

    void styledwidgetrenderer::render_button_back( const quad& dest, WidgetVisualStates look )
    {
        button_styles_[look].background_.render( dest, renderer() );
    }

    void styledwidgetrenderer::render_checkbox( const quad& q, WidgetVisualStates, const bool checked )
    {
        checkbox_.render( q, checked, renderer() );
    }

    void styledwidgetrenderer::render_button_highlight( const quad& dest, WidgetVisualStates look )
    {
        button_styles_[look].highlight_.render( dest, renderer() );
    }

    void styledwidgetrenderer::on_event( const windowchange& e, const window& w )
    {
        if( e.event_type() == windowchange::ResizeChange )
        {
            handle_screensize_changed( *e.resized_dimensions(), w );
        }
    }

    //-------------------------------------------------------------------------

    styledwidgetrenderer::tex_and_quad::tex_and_quad()
        : texhandle_( texturehandle_t() )
        , quad_()
    {
    }

    void styledwidgetrenderer::tex_and_quad::load( const TiXmlElement& xmlElem, rendering_system& rr )
    {
        const auto texName = read_attribute( xmlElem, SOLOSNAKE_TEXTURE_ELEMENT );

        deferred_renderer::screen_quad q;
        texturehandle_t h = rr.get_cache_ptr()->get_texture( texName );

        // If it contains a single BGRA, all the corners are this colour
        if( read_attribute( xmlElem, SOLOSNAKE_BGRA_ELEMENT, quad_.cornerColours[0], true ) )
        {
            q.cornerColours[1] = q.cornerColours[0];
            q.cornerColours[2] = q.cornerColours[0];
            q.cornerColours[3] = q.cornerColours[0];
        }
        else
        {
            read_attribute( xmlElem, SOLOSNAKE_BGRA0_ELEMENT, q.cornerColours[0], true );
            read_attribute( xmlElem, SOLOSNAKE_BGRA1_ELEMENT, q.cornerColours[1], true );
            read_attribute( xmlElem, SOLOSNAKE_BGRA2_ELEMENT, q.cornerColours[2], true );
            read_attribute( xmlElem, SOLOSNAKE_BGRA3_ELEMENT, q.cornerColours[3], true );
        }

        read_attribute( xmlElem, SOLOSNAKE_X0_ELEMENT, q.texCoord.x0, false );
        read_attribute( xmlElem, SOLOSNAKE_X1_ELEMENT, q.texCoord.x1, false );
        read_attribute( xmlElem, SOLOSNAKE_Y0_ELEMENT, q.texCoord.y0, false );
        read_attribute( xmlElem, SOLOSNAKE_Y1_ELEMENT, q.texCoord.y1, false );

        texhandle_ = h;
        quad_      = q;
    }

    void styledwidgetrenderer::tex_and_quad::render( const quad& dest, deferred_renderer& r ) const
    {
        if( texturehandle_t() != texhandle_ )
        {
            auto q = quad_;
            q.screenCoord = dest;
            r.draw_screen_quads( texhandle_, &q, 1 );
        }
    }

    //-------------------------------------------------------------------------

    styledwidgetrenderer::buttonstyle_layer::buttonstyle_layer() : layer_()
    {
    }

    void styledwidgetrenderer::buttonstyle_layer::load( const TiXmlElement* xmlElem,
                                                        rendering_system& rr )
    {
        // We allow this value to be null - it means the XML element was not
        // defined in the XML file.
        if( xmlElem )
        {
            tex_and_quad taq;
            taq.load( *xmlElem, rr );
            layer_ = taq;
        }
    }

    void styledwidgetrenderer::buttonstyle_layer::render( const quad& dest,
                                                          deferred_renderer& r ) const
    {
        layer_.render( dest, r );
    }

    //-------------------------------------------------------------------------

    styledwidgetrenderer::buttonstyle::buttonstyle() : fontColour_( White )
    {
    }

    void styledwidgetrenderer::buttonstyle::load_styles( buttonstyle* buttonStyles,
                                                         TiXmlElement* const pButtonsElement,
                                                         const shared_ptr<rendering_system>& rr )
    {
        const char* buttonStyleNames[SOLOSNAKE_WIDGET_VISUALSTATES_COUNT] = { 0 };

        buttonStyleNames[LooksDisabled]    = SOLOSNAKE_VISUALSTATES_DISABLED_XMLNAME;
        buttonStyleNames[LooksNormal]      = SOLOSNAKE_VISUALSTATES_NORMAL_XMLNAME;
        buttonStyleNames[LooksActivatable] = SOLOSNAKE_VISUALSTATES_HIGHLIGHTED_XMLNAME;
        buttonStyleNames[LooksActivated]   = SOLOSNAKE_VISUALSTATES_ACTIVATED_XMLNAME;

        // THESE CAN HAVE CUSTOM STYLES LATER:
        buttonStyleNames[LooksNormalAndHasKeyboardFocus]      = SOLOSNAKE_VISUALSTATES_NORMAL_XMLNAME;
        buttonStyleNames[LooksActivatableAndHasKeyboardFocus] = SOLOSNAKE_VISUALSTATES_HIGHLIGHTED_XMLNAME;
        buttonStyleNames[LooksActivatedAndHasKeyboardFocus]   = SOLOSNAKE_VISUALSTATES_ACTIVATED_XMLNAME;

        for( size_t i = 0; i < SOLOSNAKE_WIDGET_VISUALSTATES_COUNT; ++i )
        {
            auto pButtonStyle = pButtonsElement->FirstChildElement( buttonStyleNames[i] );

            if( pButtonStyle )
            {
                buttonStyles[i].load( *pButtonStyle, *rr );
            }
        }
    }

    void styledwidgetrenderer::buttonstyle::load( TiXmlElement& xmlElem, rendering_system& rr )
    {
        solosnake::read_attribute( xmlElem, SOLOSNAKE_TEXTBGRA_ELEMENT, fontColour_, true );
        // Both of these are optional.
        background_.load( xmlElem.FirstChildElement( SOLOSNAKE_BASE_ELEMENT ), rr );
        highlight_.load( xmlElem.FirstChildElement( SOLOSNAKE_OVERLAY_ELEMENT ), rr );
    }

    //-------------------------------------------------------------------------

    styledwidgetrenderer::checkbox::checkbox()
        : checked_()
        , unchecked_()
    {
    }


    void styledwidgetrenderer::checkbox::load( TiXmlElement& xmlElem, rendering_system& rr )
    {
        const TiXmlElement& checkedXml   = get_child_element( xmlElem, SOLOSNAKE_CHECKED_ELEMENT );
        const TiXmlElement& uncheckedXml = get_child_element( xmlElem, SOLOSNAKE_UNCHECKED_ELEMENT );

        checked_.load( checkedXml, rr );
        unchecked_.load( uncheckedXml, rr );
    }

    void styledwidgetrenderer::checkbox::render( const quad& q,
                                                 const bool checked,
                                                 deferred_renderer& r ) const
    {
        if( checked )
        {
            checked_.render( q, r );
        }
        else
        {
            unchecked_.render( q, r );
        }
    }

}
