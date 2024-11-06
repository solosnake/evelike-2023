#ifndef gamebackground_widgetsfactory_hpp
#define gamebackground_widgetsfactory_hpp

#include <memory>
#include "solosnake/blue/blue_widgetsfactory.hpp"

namespace solosnake
{
    class ifilefinder;
    class ilanguagetext;
    class widgetrenderingstyles;
}

namespace blue
{
    class compiler;
    class deck;
    class igameview;
    class pendinginstructions;

    //! This is the factory which creates the widgets which are available to 
	//! to a screen of type "game". A "game" screen can use XML to instance
    //! any widgets from this factory.
    class gamebackground_widgetsfactory : public widgetsfactory
    {
    public:

        //!
        gamebackground_widgetsfactory(
            const std::weak_ptr<igameview>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<solosnake::ifilefinder>& stylesPaths,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<deck>& playersdeck,
            const std::shared_ptr<pendinginstructions>&,
            const std::shared_ptr<compiler>& );

        virtual ~gamebackground_widgetsfactory();
    };
}

#endif
