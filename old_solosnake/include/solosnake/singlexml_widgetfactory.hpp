#ifndef solosnake_singlexmlwidgetfactory_hpp
#define solosnake_singlexmlwidgetfactory_hpp

#include "solosnake/ixml_widgetfactory.hpp"

namespace solosnake
{

    //! Class for creating a single type of widget.
    //! Inherit from this to simplify the widget type name
    //! association. Add several of these to a factory to create
    //! the general factory.
    class singlexml_widgetfactory : public ixml_widgetfactory
    {
    public:

        //! Specify the case sensitive name of the type of widget
        //! created by this factory. This should match the
        //! "type" attribute in the xml "widget" element.
        explicit singlexml_widgetfactory( const std::string& typeName );

        virtual ~singlexml_widgetfactory();

        const std::string& type_name() const;

        virtual std::shared_ptr<iwidget> create_widget( const TiXmlElement& xmlWidget ) const = 0;

    private:

        std::string typeName_;
    };

    //-------------------------------------------------------------------------

    inline const std::string& singlexml_widgetfactory::type_name() const
    {
        return typeName_;
    }
}

#endif
