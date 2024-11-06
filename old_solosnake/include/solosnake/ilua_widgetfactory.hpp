#ifndef solosnake_ilua_widgetfactory_hpp
#define solosnake_ilua_widgetfactory_hpp

#include <string>
#include "solosnake/ilua_widget.hpp"
#include "solosnake/singlexml_widgetfactory.hpp"
#include "solosnake/external/xml.hpp"

namespace solosnake
{
    class ilua_widgetfactory : public singlexml_widgetfactory
    {
    public:

        explicit ilua_widgetfactory( const std::string& typeName );

    protected:

        //! Called by the inheriting factory class to read the required
        //! methods for the iwidget from XML, create a standardised set
        //! of names for them, and register them with this lua instance.
        //! Note: the parent Lunar widget class may have to be registered
        //! with Lua beforehand.
        static ilua_widget::functionnames read_and_register_functions(
                lua_State*,
                const iwidgetname& name,
                const TiXmlElement& xmlWidget );
    };
}

#endif
