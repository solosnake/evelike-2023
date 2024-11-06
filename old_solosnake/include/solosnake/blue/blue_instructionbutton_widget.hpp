#ifndef blue_instructionbuttonwidget_hpp
#define blue_instructionbuttonwidget_hpp

#include <memory>
#include "solosnake/textbutton_widget.hpp"
#include "solosnake/blue/blue_iinstructionsender.hpp"
#include "solosnake/blue/blue_instructions.hpp"

namespace blue
{
    class ipendinginstructions;

    // Button on gui which owns a set of instructions which can be
    // send to the board via the ipendinginstructions when the user
    // clicks on the button.
    class instructionbuttonwidget
        : public solosnake::textbutton_widget
        , public iinstructionsender
    {
    public:

        instructionbuttonwidget(
            const solosnake::widgetparams&,
            const std::shared_ptr<ipendinginstructions>&,
            Instructions&& code,
            const std::shared_ptr<solosnake::luaguicommandexecutor>,
            const solosnake::ilua_widget::functionnames&,
            const solosnake::utf8text& textid,
            const int textsize,
            const std::shared_ptr<solosnake::iwidgetrenderer>,
            const std::shared_ptr<solosnake::ilanguagetext> );

        virtual ~instructionbuttonwidget();

    private:

        void instructions_were_aborted() override;

        void instructions_were_sent_to_board() override;

        void on_clicked( unsigned short button, solosnake::screenxy ) override;

    private:

        std::weak_ptr<ipendinginstructions> pendinginstructions_;
        std::shared_ptr<Instructions>       instructions_;
    };
}

#endif
