#ifndef solosnake_luasinglewidgetfactory_hpp
#define solosnake_luasinglewidgetfactory_hpp

#include <memory>
#include "solosnake/singlexml_widgetfactory.hpp"
#include "solosnake/external/lua.hpp"

namespace solosnake
{
    //! A type of widget factory which can create only a single type of
    //! lua widget, and which also knows how to register the type(s)
    //! used by the lua widget and Lunar. Lunar requires registration of the
    //! associated types up front.
    //! These types are the Lunar types that the widget callbacks (written in
    //! lua and defined in the XML) will interact with to communicate
    //! with the main application. They are not necessarily the widget object
    //! itself, but more likely a data type associated with a widget type.
    class luasinglewidgetfactory : public singlexml_widgetfactory
    {
    public:
        virtual void register_types_in( lua_State* ) = 0;
    };

    typedef std::shared_ptr<luasinglewidgetfactory> luasinglewidgetfactory_ptr;
}

#endif
