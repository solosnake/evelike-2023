#include "solosnake/blue/blue_actionbar_widgetfactory.hpp"
#include "solosnake/blue/blue_actionbar_widget.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/styledwidgetrenderer.hpp"
#include "solosnake/texquad.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/external/xml.hpp"
#include <cassert>

using namespace std;
using namespace solosnake;

#define SOLOSNAKE_ACTIONBAR_ELEMENT         "actionbar"
#define SOLOSNAKE_AB_BACKGROUND_ELEMENT     "background"
#define SOLOSNAKE_AB_BORDERSIZE_ELEMENT     "bordersize"
#define SOLOSNAKE_AB_BUTTONSIZE_ELEMENT     "buttonsize"
#define SOLOSNAKE_AB_NORMAL_ELEMENT         "normal"
#define SOLOSNAKE_AB_DISABLED_ELEMENT       "disabled"
#define SOLOSNAKE_AB_HIGHLIGHTED_ELEMENT    "highlighted"
#define SOLOSNAKE_AB_BGRA_ELEMENT           "bgra"

namespace blue
{
    namespace
    {
        styledwidgetrenderer::tex_and_quad
        load_actionbar_background_from_xml( TiXmlElement& actionbarElement, rendering_system& rs )
        {
            styledwidgetrenderer::tex_and_quad result;

            result.load( get_child_element( actionbarElement, SOLOSNAKE_AB_BACKGROUND_ELEMENT ), rs );

            return result;
        }

        void load_actionbar_button_colours( TiXmlElement& actionbarElement, actionbar_params& params )
        {
            auto child = actionbarElement.FirstChildElement( SOLOSNAKE_AB_NORMAL_ELEMENT );
            if( child )
            {
                read_attribute( *child, SOLOSNAKE_AB_BGRA_ELEMENT, params.normal_colour_, false );
            }

            child = actionbarElement.FirstChildElement( SOLOSNAKE_AB_DISABLED_ELEMENT );
            if( child )
            {
                read_attribute( *child, SOLOSNAKE_AB_BGRA_ELEMENT, params.disabled_colour_, false );
            }

            child = actionbarElement.FirstChildElement( SOLOSNAKE_AB_HIGHLIGHTED_ELEMENT );
            if( child )
            {
                read_attribute( *child, SOLOSNAKE_AB_BGRA_ELEMENT, params.highlighted_colour_, false );
            }
        }

        actionbar_params
        load_actionbar_params_from_xml( const string& stylename,
                                        const ifilefinder& stylesPaths,
                                        const char* const styleElement,
                                        rendering_system& rs )
        {
            actionbar_params result;

            auto styleUrl = stylesPaths.get_file( stylename );
            assert( filepath_exists( styleUrl ) );

            TiXmlDocument doc;
            load_xml_doc( doc, styleUrl.string().c_str() );

            auto styleNode = doc.FirstChildElement( styleElement );

            if( styleNode == nullptr )
            {
                ss_throw( "Missing style node." );
            }

            if( styleNode->NextSiblingElement( styleElement ) != nullptr )
            {
                ss_throw( "Invalid XML, only one style must be defined per document." );
            }

            auto actionbarElement = get_child_element( *styleNode, SOLOSNAKE_ACTIONBAR_ELEMENT );

            if( actionbarElement.NextSiblingElement( SOLOSNAKE_ACTIONBAR_ELEMENT ) != nullptr )
            {
                ss_throw( "Invalid XML, only one actionbar must be defined per document." );
            }

            auto background = load_actionbar_background_from_xml( actionbarElement, rs );
            result.background_tex_handle_ = background.texhandle_;
            result.background_tex_coords_ = background.quad_.texCoord;

            read_attribute( actionbarElement, SOLOSNAKE_AB_BORDERSIZE_ELEMENT, result.border_width_,      false );
            read_attribute( actionbarElement, SOLOSNAKE_AB_BUTTONSIZE_ELEMENT, result.button_sidelength_, false );

            load_actionbar_button_colours( actionbarElement, result );

            return result;
        }
    }

    actionbar_widgetfactory::actionbar_widgetfactory(
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles,
        const shared_ptr<ifilefinder>& stylesPaths,
        const shared_ptr<deck>& playersdeck)
        : ilua_widgetfactory( "actionbar" )
        , lce_( lce )
        , renderingStyles_( styles )
        , stylesPaths_( stylesPaths )
        , deck_( playersdeck )
    {
        assert( playersdeck );
        assert( stylesPaths );

        if( ! lunar<actionbar_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<actionbar_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget> actionbar_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // The actionbar does not use the XML to set its location or size directly.
        const auto name = read_name( xmlWidget );
        const auto z    = read_zlayer( xmlWidget );
        const auto tab  = read_tab_order_value( xmlWidget );

        const auto stylename = ixml_widgetfactory::read_style( xmlWidget );
        auto widgetrender = renderingStyles_->get_widget_rendering_style( stylename );

        // The iwidgetrenderer is too base for the needs of the actionbar, a class specific to
        // the game. So we use the stylename to look up and re-open the XML style file to read
        // the data that the widgetrenderingstyles could not understand (and so it skips).
        auto barparams = load_actionbar_params_from_xml( stylename,
                                                         *stylesPaths_,
                                                         ixml_widgetfactory::AttributeStyle,
                                                         widgetrender->renderingsystem() );

        const auto fnames = ilua_widgetfactory::read_and_register_functions( lce_->lua(),
                                                                             name,
                                                                             xmlWidget );

        return make_shared<actionbar_widget>( deck_,
                                              barparams,
                                              widgetparams( name, rect( 0, 0, 1u, 1u ), z, tab ),
                                              fnames,
                                              lce_,
                                              widgetrender );
    }
}
