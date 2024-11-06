#include "solosnake/list_widgetfactory.hpp"
#include "solosnake/list_widget.hpp"
#include "solosnake/iwidgetname.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace std;

namespace solosnake
{
    list_widgetfactory::list_widgetfactory( const shared_ptr<luaguicommandexecutor>& lce,
                                            const shared_ptr<widgetrenderingstyles>& rs )
        : ilua_widgetfactory( "filelist" )
        , lce_( lce )
        , renderingStyles_( rs )
    {
        assert( lce );
        assert( rs );

        if( false == lunar<list_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<list_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    shared_ptr<iwidget> list_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        const auto folderAtttribute = "folder";

        // This can throw if some params are missing or out of range.
        const auto params    = read_widgetparams( xmlWidget );
        const auto folder    = read_attribute( xmlWidget, folderAtttribute );
        const auto stylename = ixml_widgetfactory::read_style( xmlWidget );
        const auto renderer  = renderingStyles_->get_widget_rendering_style( stylename );
        const auto fnames    = ilua_widgetfactory::read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        shared_ptr<vector<std::string>> items;

        return list_widget::make_filelist_widget( items, params, lce_, renderer, fnames );
    }
}
