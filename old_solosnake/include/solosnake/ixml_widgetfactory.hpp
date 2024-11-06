#ifndef solosnake_ixml_widgetfactory_hpp
#define solosnake_ixml_widgetfactory_hpp

#include <memory>
#include <string>
#include <iosfwd>
#include "solosnake/iwidget.hpp"
#include "solosnake/external/xml.hpp"

namespace solosnake
{
    //! Factory interface for construction of widgets from XML (using TinyXML).
    class ixml_widgetfactory
    {
    public:

        virtual ~ixml_widgetfactory();

        virtual std::shared_ptr<iwidget> create_widget( const TiXmlElement& xmlWidget ) const = 0;

    protected:

        std::string read_name( const TiXmlElement& xmlWidget ) const;

        std::string read_type( const TiXmlElement& xmlWidget ) const;

        //! Read optional style. Default is empty string.
        std::string read_style( const TiXmlElement& xmlWidget ) const;

        //! Reads the attributes or throws if they are not present or contain
        //! invalid values. Widget dimensions are specified in pixels.
        void read_common_attributes( const TiXmlElement& xmlWidget,
                                     int& x,
                                     int& y,
                                     int& width,
                                     int& height,
                                     std::string& style ) const;

        //! Returns zero if this optional element is not present.
        int read_zlayer( const TiXmlElement& xmlWidget ) const;

        //! Returns default tab order if this optional element is not present.
        int read_tab_order_value( const TiXmlElement& xmlWidget ) const;

        bool is_widget( const TiXmlElement& xmlWidget ) const;

        //! Fills in optional params with defaults.
        widgetparams read_widgetparams( const TiXmlElement& ) const;

    protected:

        static const int DefaultTabOrderValue = 0;

        //! The attribute identifying the XML element as being a widget object.
        static const char AttributeWidget[];

        //! The attribute associated with the type of the widget.
        //! This is used by the factory class in choosing how to
        //! construct the widget when reading the XML.
        static const char AttributeType[];

        //! The name to give the widget. This will be used to
        //! reference the object in script and should be unique
        //! per screen widget.
        static const char AttributeName[];

        //! The (optional) attribute for the widgets layer. Zero is default.
        static const char AttributeZLayer[];

        //! The X location for widgets that require this.
        static const char AttributeX[];

        //! The Y location for widgets that require this.
        static const char AttributeY[];

        //! The widget width attribute for widgets that require this.
        static const char AttributeWidth[];

        //! The widget height attribute for widgets that require this.
        static const char AttributeHeight[];

        //! The name of the style to use when rendering the widget. Empty means the
        //! default style.
        static const char AttributeStyle[];
    };
}

#endif
