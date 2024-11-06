#include <cassert>
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/blue/blue_mapedit_widget.hpp"
#include "solosnake/blue/blue_mapedit_widgetfactory.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/blue/blue_widgetsfactory.hpp"
#include "solosnake/throw.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    mapedit_widgetfactory::mapedit_widgetfactory(
        const std::shared_ptr<mapedit>& sharedmap,
        const std::shared_ptr<user_settings>& settings,
        const std::shared_ptr<solosnake::luaguicommandexecutor>& lce,
        const std::shared_ptr<solosnake::widgetrenderingstyles>& renderingStyle,
        const std::shared_ptr<solosnake::ilanguagetext>& lang )
        : ilua_widgetfactory( "mapedit" )
        , settings_( settings )
        , lce_( lce )
        , rendering_styles_( renderingStyle )
        , language_( lang )
        , sharedmap_( sharedmap )
    {
        assert( settings_.get() );

        // They are registered within lua as textbutton_widgets
        if( !lunar<mapedit_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<mapedit_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    //! Creates a mapedit_widget.
    std::shared_ptr<solosnake::iwidget>
    mapedit_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        const auto widgetnname  = ixml_widgetfactory::read_name( xmlWidget );
        const auto z            = ixml_widgetfactory::read_zlayer( xmlWidget );
        const auto tab          = ixml_widgetfactory::read_tab_order_value( xmlWidget );
        const auto stylename    = ixml_widgetfactory::read_style( xmlWidget );
        const auto widgetrender = rendering_styles_->get_widget_rendering_style( stylename );
        const auto fnames       = ilua_widgetfactory::read_and_register_functions( lce_->lua(),
                                                                                   widgetnname,
                                                                                   xmlWidget );

        return make_shared<mapedit_widget>( widgetparams( widgetnname, rect(), z, tab ),
                                            *settings_,
                                            sharedmap_,
                                            lce_,
                                            widgetrender,
                                            fnames );
    }


    mapeditscreen_widgetsfactory::mapeditscreen_widgetsfactory(
        const std::shared_ptr<mapedit_widgetfactory>& mapeditwidgets,
        const std::shared_ptr<widgetsfactory>& menuwidgets,
        const std::shared_ptr<solosnake::luaguicommandexecutor>& lce )
        : widgetsfactory( lce )
        , mapeditwidgets_( mapeditwidgets )
        , menuwidgets_( menuwidgets )
    {
    }

    std::shared_ptr<solosnake::iwidget>
    mapeditscreen_widgetsfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        if( !is_widget( xmlWidget ) )
        {
            ss_throw( "Non widget element encountered in XML." );
        }

        const std::string widgetype = ixml_widgetfactory::read_type( xmlWidget );

        if( widgetype != mapeditwidgets_->type_name() )
        {
            return menuwidgets_->create_widget( xmlWidget );
        }
        else
        {
            return mapeditwidgets_->create_widget( xmlWidget );
        }
    }
}
