#include "solosnake/blue/blue_instructionbutton_widget.hpp"
#include "solosnake/blue/blue_ipendinginstructions.hpp"
#include "solosnake/blue/blue_instructions.hpp"

using namespace solosnake;

namespace blue
{
    instructionbuttonwidget::instructionbuttonwidget(
        const widgetparams& params,
        const std::shared_ptr<ipendinginstructions>& pender,
        instructions&& code,
        const std::shared_ptr<luaguicommandexecutor> lce,
        const ilua_widget::functionnames& fnames,
        const utf8text& textid,
        const int textsize,
        const std::shared_ptr<iwidgetrenderer> renderingStyle,
        const std::shared_ptr<ilanguagetext> language )
        : textbutton_widget( params,
                             lce,
                             fnames,
                             textid,
                             textsize,
                             renderingStyle,
                             language )
        , pendinginstructions_( pender )
        , instructions_( std::make_shared<instructions>( std::move( code ) ) )
    {
        this->enable_widget();
    }

    instructionbuttonwidget::~instructionbuttonwidget()
    {
        auto pending = pendinginstructions_.lock();
        if( pending )
        {
            pending->cancel_pending_instructions_for( this );
        }
    }

    void instructionbuttonwidget::on_clicked( unsigned short button, screenxy xy )
    {
        assert( !pendinginstructions_.expired() );
        textbutton_widget::on_clicked( button, xy );

        auto pending = pendinginstructions_.lock();
        if( pending )
        {
            pending->set_pending_instructions( this, instructions_ );
        }
    }

    void instructionbuttonwidget::instructions_were_aborted()
    {
    }

    void instructionbuttonwidget::instructions_were_sent_to_board()
    {
    }
}
