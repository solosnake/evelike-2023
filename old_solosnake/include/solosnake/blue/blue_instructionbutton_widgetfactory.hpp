#ifndef blue_instructionbuttonwidgetfactory_hpp
#define blue_instructionbuttonwidgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class luaguicommandexecutor;
    class widgetrenderingstyles;
}

namespace blue
{
    class ipendinginstructions;
    class compiler;
    class Instructions;

    class instructionbuttonwidgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        instructionbuttonwidgetfactory(
            const std::shared_ptr<ipendinginstructions>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<compiler>&);

        std::shared_ptr<solosnake::iwidget> create_widget(const TiXmlElement&) const override;

        static Instructions read_instructions(const TiXmlElement&, const compiler&);

        static const char AttributeInstruction[];
        static const char AttributeInstructions[];

    private:

        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   renderingStyles_;
        std::shared_ptr<solosnake::ilanguagetext>           language_;
        std::shared_ptr<ipendinginstructions>               pendinginstructions_;
        std::shared_ptr<compiler>                           compiler_;
    };
}

#endif
