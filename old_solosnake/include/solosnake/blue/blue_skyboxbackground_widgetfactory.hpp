#ifndef blue_skyboxbackground_widget_factory_hpp
#define blue_skyboxbackground_widget_factory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class widgetrenderingstyles;
    class luaguicommandexecutor;
}

namespace blue
{
    class datapaths;

    //! Factory class which creates a skybox background widget.
    class skyboxbackground_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        skyboxbackground_widgetfactory(
            const std::shared_ptr<datapaths>& paths,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>& );

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<datapaths>                          datapaths_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
    };
}

#endif
