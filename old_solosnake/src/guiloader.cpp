#include "solosnake/gui.hpp"
#include "solosnake/guiloader.hpp"

namespace solosnake
{
    guiloader::guiloader( const std::shared_ptr<ixml_widgetfactory>& maker )
        : ixmlelementreader( SOLOSNAKE_XML_ELEMENT_GUI )
        , widget_( "widget" )
        , widgetType_( "type" )
        , widgetMaker_( maker )
    {
    }

    //! Ensure the dtor knows about the types of iwidget by declaring an
    //! explicit instance here.
    guiloader::~guiloader()
    {
        widgets_.clear();
    }

    void guiloader::move_widgets_to( std::list<std::shared_ptr<iwidget>>& other )
    {
        other = std::move( widgets_ );
    }

    bool guiloader::can_read_version( const std::string& v ) const
    {
        return v == "1.0";
    }

    bool guiloader::read_from_element( const TiXmlElement& elem )
    {
        bool createdWidget = false;

        if( elem.ValueStr() == widget_ )
        {
            widgets_.emplace( widgets_.begin(), widgetMaker_->create_widget( elem ) );
            createdWidget = true;
        }

        return createdWidget;
    }
}
