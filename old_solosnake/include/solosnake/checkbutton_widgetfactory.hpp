#ifndef solosnake_checkbutton_widgetfactory_hpp
#define solosnake_checkbutton_widgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class luaguicommandexecutor;
    class widgetrenderingstyles;

    //! Factory class for creating (Lua) check button widget types from XML.
    //! A check button displays a centred piece of text, using a font and optional
    //! text scaling, with a tickbox that can be toggled to ticked or unticked.
    class checkbutton_widgetfactory : public ilua_widgetfactory
    {
    public:

        checkbutton_widgetfactory(
            std::shared_ptr<luaguicommandexecutor>,
            std::shared_ptr<widgetrenderingstyles>,
            std::shared_ptr<ilanguagetext>);

        std::shared_ptr<iwidget> create_widget(const TiXmlElement&) const override;

        static const char AttributeTextId[];
        static const char AttributeTextSize[];
        static const char AttributeChecked[];

    private:

        std::shared_ptr<luaguicommandexecutor>  lce_;
        std::shared_ptr<widgetrenderingstyles>  renderingStyles_;
        std::shared_ptr<ilanguagetext>          language_;
    };
}

#endif
