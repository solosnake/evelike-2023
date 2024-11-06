#ifndef blue_gamebackground_widget_factory_hpp
#define blue_gamebackground_widget_factory_hpp

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

    //! Factory class which creates just the (Lua) board background widget type
    //! from an XML element.
    class gamebackground_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        //! iboardview is guaranteed to persist for the lifetime of 
        gamebackground_widgetfactory(
            const std::weak_ptr<igameview>&,
            const std::shared_ptr<pendinginstructions>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<player>& );

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<pendinginstructions>                pendinginstructions_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
        std::shared_ptr<player>                             player_;
        std::weak_ptr<igameview>                            gameview_;
    }; 
}

#endif
