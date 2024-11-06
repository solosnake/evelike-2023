#ifndef blue_codeeditor_widgetfactory_hpp
#define blue_codeeditor_widgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class widgetrenderingstyles;
}

namespace blue
{
    class compiler;

    //! Factory class which creates just the (Lua) code editor
    //! widget type from an XML element.
    class codeeditor_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        codeeditor_widgetfactory(
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<compiler>& );

        virtual ~codeeditor_widgetfactory();

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
        std::shared_ptr<compiler>                           compiler_;
    };
}

#endif
