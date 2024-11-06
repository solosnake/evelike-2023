#ifndef solosnake_backgroundwidgetfactory_hpp
#define solosnake_backgroundwidgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class widgetrenderingstyles;
    class luaguicommandexecutor;

    //! Factory class which creates just the (Lua) background widget type
    //! from an XML element.
    class background_widgetfactory : public ilua_widgetfactory
    {
    public:

        explicit background_widgetfactory( 
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<widgetrenderingstyles>& );

        std::shared_ptr<iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<luaguicommandexecutor> lce_;
        std::shared_ptr<widgetrenderingstyles> renderingStyles_;
    };
}

#endif
