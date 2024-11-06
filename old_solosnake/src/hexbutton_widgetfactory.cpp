#include <cassert>
#include "solosnake/ilanguagetext.hpp"
#include "solosnake/hexbutton_widgetfactory.hpp"
#include "solosnake/polygonshape.hpp"
#include "solosnake/textbutton_widget.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        unique_ptr<ishape> make_hexagon_shape()
        {
            // Build a polygon with a hexagon inside the unit square 0,0 to 1,1.
            unique_ptr<polygonshape> hexagon( new polygonshape() );

            const float root3over4 = sqrt( 3.0f ) / 4.0f;

            // Points as they appear moving clockwise around the hexagon.
            const point2_t p0 = { 0.00f, 0.5f };
            const point2_t p1 = { 0.25f, 0.5f - root3over4 };
            const point2_t p2 = { 0.75f, 0.5f - root3over4 };
            const point2_t p3 = { 1.00f, 0.5f };
            const point2_t p4 = { 0.75f, 0.5f + root3over4 };
            const point2_t p5 = { 0.25f, 0.5f + root3over4 };

            // 4 triangles, in a fan from p0.
            hexagon->add_triangle( p0, p1, p2 );
            hexagon->add_triangle( p0, p2, p3 );
            hexagon->add_triangle( p0, p3, p4 );
            hexagon->add_triangle( p0, p4, p5 );

            return unique_ptr<ishape>( hexagon.release() );
        }
    }

    hexbutton_widgetfactory::hexbutton_widgetfactory(
            const shared_ptr<luaguicommandexecutor>& lce,
            const shared_ptr<widgetrenderingstyles>& styles,
            const shared_ptr<ilanguagetext>& language )
        : ilua_widgetfactory( "hexbutton" )
        , lce_( lce )
        , renderingStyles_( styles )
        , language_( language )
    {
        if( false == lunar<textbutton_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<textbutton_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget>
    hexbutton_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        const widgetparams params = ixml_widgetfactory::read_widgetparams( xmlWidget );

        // Read optional UTF-8 "text"
        string textid;
        solosnake::read_attribute( xmlWidget, textbutton_widgetfactory::AttributeTextId, textid, true );

        // read optional text size (default is zero)
        int textsize = 0;
        solosnake::read_attribute( xmlWidget, textbutton_widgetfactory::AttributeTextSize, textsize, true );

        const string stylename = ixml_widgetfactory::read_style( xmlWidget );
        auto widgetrender = renderingStyles_->get_widget_rendering_style( stylename );

        const auto fnames = ilua_widgetfactory::read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        return make_shared<textbutton_widget>( 
            params, make_hexagon_shape(), lce_, fnames, textid, textsize, widgetrender, language_ );
    }
}
