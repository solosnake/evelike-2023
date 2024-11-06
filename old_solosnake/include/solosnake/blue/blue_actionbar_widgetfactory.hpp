#ifndef blue_actionbar_widgetfactory_hpp
#define blue_actionbar_widgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class ifilefinder;
    class luaguicommandexecutor;
    class widgetrenderingstyles;
}

namespace blue
{
    class compiler;
    class deck;
    class pendinginstructions;

    //! Factory class which creates just the (Lua) Machine action bar
    //! widget type from an XML element.
    class actionbar_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        actionbar_widgetfactory(
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<solosnake::ifilefinder>& stylesPaths,
            const std::shared_ptr<deck>& playersdeck );

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
        std::shared_ptr<solosnake::ifilefinder>             stylesPaths_;
        std::shared_ptr<deck>                               deck_;
    };
}

#endif
