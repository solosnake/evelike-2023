#ifndef ixmlwidgetreader_hpp
#define ixmlwidgetreader_hpp

#include <list>
#include <memory>
#include "solosnake/game/ixmlwidgetfactory.hpp"

namespace solosnake
{
    //! Interface to reading the xml widgets from XML into a collection.
    //! This allows us to batch the reading of the widgets and perform
    //! an action upon completing a read.
    class ixmlwidgetreader
    {
    public:
        explicit ixmlwidgetreader( std::tr1::shared_ptr<ixmlwidgetfactory> );

        virtual void swap( std::list<std::unique_ptr<iwidget>>& other ) = 0;

        virtual bool read_from_element( TiXmlElement& widgetDefinition ) = 0;

        virtual ~ixmlwidgetreader();
    };
}

#endif
