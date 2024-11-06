#include "solosnake/xmlwidgetsmaker.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    void xmlwidgetsmaker::register_widget_factory( std::shared_ptr<singlexml_widgetfactory> f )
    {
        auto i = factories_.find( f->type_name() );

        if( i == factories_.end() )
        {
            factories_[f->type_name()] = f;
        }
        else
        {
            ss_err( "Widget factory already registered : ", f->type_name() );
            ss_throw( "A singlexml_widgetfactory tried to register using an already "
                      "registered name." );
        }
    }

    //! Examines the xml element for the type of the widget to create (contained in
    //! the "type" attrib.)
    std::shared_ptr<iwidget> xmlwidgetsmaker::create_widget( const TiXmlElement& xml ) const
    {
        if( !is_widget( xml ) )
        {
            ss_throw( "Non widget element encountered in XML." );
        }

        std::string widgetype = ixml_widgetfactory::read_type( xml );

        auto i = factories_.find( widgetype );

        if( i == factories_.end() )
        {
            ss_err( "Unable to find factory for widget type : ", widgetype );
            ss_throw( "No factory for widget type." );
        }

        return i->second->create_widget( xml );
    }
}
