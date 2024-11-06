#ifndef blue_networkgamescreen_hpp
#define blue_networkgamescreen_hpp

#include <memory>
#include <string>
#include "solosnake/blue/blue_screen.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/iscreen_factory.hpp"

namespace blue
{
    class user_settings;
    class translator;

    //! The main choice screen for the app. GUI driven. This is a mini application
    //! which contains the game itself.
    class gamescreen : public screen
    {
    public:

        static std::shared_ptr<solosnake::iscreen_factory> make_factory(
            const std::shared_ptr<solosnake::ioswindow>&,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>& );

        gamescreen(
            const std::string& xmlFile,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>&);

        //! Starts the screen, a mini application, running.
        solosnake::nextscreen show_screen( const std::shared_ptr<solosnake::ioswindow>& ) override;

    private:

        class screenfactory;
        friend class gamescreen::screenfactory;

        solosnake::nextscreen run_game_as_host(
            const std::shared_ptr<solosnake::ioswindow>& wnd,
            const unsigned long tickLengthMs,
            const unsigned short timeout,
            const unsigned int n_participants,
            const unsigned short host_port) const;

        solosnake::nextscreen run_game_as_client(
            const std::shared_ptr<solosnake::ioswindow>& wnd,
            const unsigned long tickLength,
            const unsigned short timeout,
            const unsigned int n_participants,
            const unsigned short host_port,
            const std::string& host_ip) const;
    };
}

#endif
