#include "solosnake/ixml_widgetfactory.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    const char ixml_widgetfactory::AttributeType[]   = "type";
    const char ixml_widgetfactory::AttributeName[]   = "name";
    const char ixml_widgetfactory::AttributeWidget[] = "widget";
    const char ixml_widgetfactory::AttributeZLayer[] = "zlayer";
    const char ixml_widgetfactory::AttributeX[]      = "x";
    const char ixml_widgetfactory::AttributeY[]      = "y";
    const char ixml_widgetfactory::AttributeWidth[]  = "width";
    const char ixml_widgetfactory::AttributeHeight[] = "height";
    const char ixml_widgetfactory::AttributeStyle[]  = "style";

    ixml_widgetfactory::~ixml_widgetfactory()
    {
    }

    std::string ixml_widgetfactory::read_name( const TiXmlElement& xml ) const
    {
        return read_attribute( xml, ixml_widgetfactory::AttributeName );
    }

    std::string ixml_widgetfactory::read_type( const TiXmlElement& xml ) const
    {
        return read_attribute( xml, ixml_widgetfactory::AttributeType );
    }

    std::string ixml_widgetfactory::read_style( const TiXmlElement& xml ) const
    {
        return read_attribute( xml, ixml_widgetfactory::AttributeStyle, true );
    }

    int ixml_widgetfactory::read_zlayer( const TiXmlElement& xml ) const
    {
        // Reads the optional z layer attribute, returning zero (default)
        // if it's not found.
        assert( xml.ValueStr() == ixml_widgetfactory::AttributeWidget );
        assert( xml.Attribute( ixml_widgetfactory::AttributeType ) );

        int z = 0;
        xml.Attribute( std::string( ixml_widgetfactory::AttributeZLayer ), &z );

        return z;
    }

    int ixml_widgetfactory::read_tab_order_value( const TiXmlElement& xml ) const
    {
        // Reads the optional tab order value attribute, returning the default
        // if it's not found.
        assert( xml.ValueStr() == ixml_widgetfactory::AttributeWidget );
        assert( xml.Attribute( ixml_widgetfactory::AttributeType ) );

        int tabOrderValue = ixml_widgetfactory::DefaultTabOrderValue;
        xml.Attribute( std::string( ixml_widgetfactory::AttributeZLayer ), &tabOrderValue );

        return tabOrderValue;
    }

    void ixml_widgetfactory::read_common_attributes(
        const TiXmlElement& xmlWidget, int& x, int& y, int& w, int& h, std::string& style ) const
    {
        if( ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeX, &x ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeY, &y ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeWidth, &w ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeHeight, &h ) ) )
        {
            ss_throw( "widget missing expected attributes (x, y, width and/or height are missing)." );
        }

        style = read_style( xmlWidget );

        if( w < 0 || h < 0 )
        {
            ss_throw( "widget width and/or height attributes are out of bounds." );
        }
    }

    widgetparams ixml_widgetfactory::read_widgetparams( const TiXmlElement& xmlWidget ) const
    {
        int x;
        int y;
        int width;
        int height;

        if( ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeX, &x ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeY, &y ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeWidth, &width ) ) ||
            ( TIXML_SUCCESS != xmlWidget.QueryIntAttribute( ixml_widgetfactory::AttributeHeight, &height ) ) )
        {
            ss_throw( "widget missing expected attributes (x, y, width and/or height are missing)." );
        }

        if( width < 0 || height < 0 )
        {
            ss_throw( "widget width and/or height attributes are out of bounds." );
        }

        const auto name = read_name( xmlWidget );
        const auto z    = read_zlayer( xmlWidget );
        const auto tab  = read_tab_order_value( xmlWidget );

        return widgetparams( name, rect(x,y,width,height), z, tab );
    }

    bool ixml_widgetfactory::is_widget( const TiXmlElement& xml ) const
    {
        return xml.ValueStr() == ixml_widgetfactory::AttributeWidget;
    }
}
