#include "solosnake/blue/blue_widgetsfactory.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/throw.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    widgetsfactory::widgetsfactory( const shared_ptr<luaguicommandexecutor>& lce )
        : lua_( lce )
    {
        assert( lce );

        if( !lce )
        {
            ss_throw( "gamebackground_widgetsfactory constructed with "
                      "empty std::shared_ptr<luaguicommandexecutor>." );
        }
    }

    widgetsfactory::~widgetsfactory()
    {
    }

    shared_ptr<iwidget> widgetsfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        return widgets_maker_.create_widget( xmlWidget );
    }

    void widgetsfactory::register_widget_factory( const shared_ptr<solosnake::singlexml_widgetfactory>& f )
    {
        widgets_maker_.register_widget_factory( f );
    }

    shared_ptr<solosnake::luaguicommandexecutor> widgetsfactory::get_luaguicommandexecutor() const
    {
        return lua_;
    }
}
