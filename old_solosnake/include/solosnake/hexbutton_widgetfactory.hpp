#ifndef solosnake_hexbuttonwidgetfactory_hpp
#define solosnake_hexbuttonwidgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class luaguicommandexecutor;
    class widgetrenderingstyles;

    //! Factory class for creating (Lua) hexagonal shaped text picturebuttonwidget types from XML.
    //! A text button displays a centred piece of text, using a font and optional text scaling,
    //! with a hexagonal ishape (the user should use a hexagonal texture with this widget).
    //! This is implemented as a textbutton_widget with a horizontally aligned ishape inside.
    class hexbutton_widgetfactory : public ilua_widgetfactory
    {
    public:

        hexbutton_widgetfactory(
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<widgetrenderingstyles>&,
            const std::shared_ptr<ilanguagetext>& );

        std::shared_ptr<iwidget> create_widget( const TiXmlElement& ) const override;

    private:
        std::shared_ptr<luaguicommandexecutor>  lce_;
        std::shared_ptr<widgetrenderingstyles>  renderingStyles_;
        std::shared_ptr<ilanguagetext>          language_;
    };
}

#endif
