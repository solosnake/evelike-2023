#ifndef blue_widgets_factory_hpp
#define blue_widgets_factory_hpp

#include <memory>
#include "solosnake/ixml_widgetfactory.hpp"
#include "solosnake/xmlwidgetsmaker.hpp"

namespace solosnake
{
    class luaguicommandexecutor;
}

namespace blue
{
    //! Base class factory for the widgets used in the blue app windows.
    class widgetsfactory : public solosnake::ixml_widgetfactory
    {
    public:

        explicit widgetsfactory( const std::shared_ptr<solosnake::luaguicommandexecutor>& );

        virtual ~widgetsfactory();

        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

        std::shared_ptr<solosnake::luaguicommandexecutor> get_luaguicommandexecutor() const;

    protected:

        void register_widget_factory( const std::shared_ptr<solosnake::singlexml_widgetfactory>& );

    private:

        std::shared_ptr<solosnake::luaguicommandexecutor>   lua_;
        solosnake::xmlwidgetsmaker                          widgets_maker_;
    };
}

#endif
