#ifndef blue_menuscreen_hpp
#define blue_menuscreen_hpp

#include "solosnake/blue/blue_screen.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

namespace blue
{
    //! The screen type created when the XML file specifies 'menu'. This is
    //! a generic screen. A screen is a mini app that is created and displayed,
    //! and which upon exiting tells the app which screen should be displayed
    //! next. A screen has a 'type', and an XML file specifying how that type
    //! should be created. A menu screen is primarily for displaying widgets
    //! and can be used for many screen types that do not need the game to be
    //! active.
    //!
    //! By convention we also add a language object to each screen.
    //!
    //! Only widgets appropriate for the menuscreen can be created in a menuscreen.
    //!
    class menuscreen : public screen
    {
    public:

        static std::shared_ptr<solosnake::iscreen_factory> make_factory(
            const std::shared_ptr<solosnake::ioswindow>&,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>& );

        solosnake::nextscreen show_screen( const std::shared_ptr<solosnake::ioswindow>& ) override;

        menuscreen(
            const std::string& xmlFile,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>&);

    private:

        class screenfactory;
        friend class menuscreen::screenfactory;
    };
}

#endif
