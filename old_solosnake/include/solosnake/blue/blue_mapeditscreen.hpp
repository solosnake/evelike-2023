#ifndef blue_mapeditscreen_hpp
#define blue_mapeditscreen_hpp

#include "solosnake/blue/blue_screen.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

namespace blue
{
    //! The screen type created when the XML file specifies 'mapedit'. A screen is a mini app that 
    //! is created and displayed, and which upon exiting tells the app which screen should be 
    //! displayed next. A screen has a 'type', and an XML file specifying how that type should be 
    //! created.
    //!
    //! By convention we also add a language object to each screen.
    //!
    //! Only widgets appropriate for the mapeditscreen can be created in a mapedit screen.
    //!
    class mapeditscreen : public screen
    {
    public:

        static std::shared_ptr<solosnake::iscreen_factory> make_factory(
            const std::shared_ptr<solosnake::ioswindow>&,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>& );

        solosnake::nextscreen show_screen( 
            const std::shared_ptr<solosnake::ioswindow>&) override;

        mapeditscreen(
            const std::string& xmlFile,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>&);

    private:

        class screenfactory;
        friend class mapeditscreen::screenfactory;
    };
}

#endif
