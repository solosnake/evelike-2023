#ifndef blue_boardbackground_widget_hpp
#define blue_boardbackground_widget_hpp

#include <string>
#include "solosnake/blue/blue_gameviewing_widget.hpp"
#include "solosnake/point.hpp"

namespace blue
{
    class igameview;
    class player;
    class pendinginstructions;

    //! Lua background widget which passes on board events.
    //! Note, a background widget will get focus by default.
    class gamebackground_widget : public gameviewing_widget
    {
    public:

        LUNAR_CLASS( gamebackground_widget );

        gamebackground_widget( lua_State* );

        gamebackground_widget(
            const std::string& name,
            const std::weak_ptr<igameview>&,
            const std::shared_ptr<player>&,
            const std::shared_ptr<pendinginstructions>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::iwidgetrenderer>&,
            const solosnake::ilua_widget::functionnames& );

    private:

        void render( const unsigned long ) const override;

        void on_clicked_on_board( const Hex_coord& ) override;

        void send_pending_instructions_to_board( Hex_coord );

    private:

        std::weak_ptr<pendinginstructions>          pendinginstructions_;
        std::shared_ptr<player>                     player_;
    };
}

#endif
