#ifndef blue_codeinspector_widgetfactory_hpp
#define blue_codeinspector_widgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class widgetrenderingstyles;
    class luaguicommandexecutor;
}

namespace blue
{
    class igameview;
    class pendinginstructions;
    class player;
    class compiler;

    //! Factory class which creates just the (Lua) Machine code inspector
    //! widget type from an XML element.
    class codeinspector_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        codeinspector_widgetfactory(
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::weak_ptr<igameview>&,
            const std::shared_ptr<player>&,
            const std::shared_ptr<compiler>& );

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<pendinginstructions>                pendinginstructions_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
        std::shared_ptr<player>                             player_;
        std::shared_ptr<compiler>                           compiler_;
        std::weak_ptr<igameview>                            gameview_;
    };
}

#endif
