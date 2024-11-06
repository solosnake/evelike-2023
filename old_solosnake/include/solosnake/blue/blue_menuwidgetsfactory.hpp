#ifndef blue_menuwidgets_factory_hpp
#define blue_menuwidgets_factory_hpp

#include <memory>
#include "solosnake/blue/blue_widgetsfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class widgetrenderingstyles;
}

namespace blue
{
    class user_settings;

    //! Factory for the widgets used in the blue app menu windows.
    class menuwidgetsfactory : public widgetsfactory
    {
    public:

        //! Factory will not produce any types of widgets that rely on igame.
        menuwidgetsfactory(
            const std::shared_ptr<user_settings>& settings,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<solosnake::ilanguagetext>& );

        virtual ~menuwidgetsfactory();
    };
}

#endif
