#ifndef solosnake_xmlwidgetsmaker_hpp
#define solosnake_xmlwidgetsmaker_hpp
#include <map>
#include <string>
#include "solosnake/ixml_widgetfactory.hpp"
#include "solosnake/singlexml_widgetfactory.hpp"

namespace solosnake
{
    //! Class for creating several types of widgets.
    class xmlwidgetsmaker : public ixml_widgetfactory
    {
    public:
        void register_widget_factory( std::shared_ptr<singlexml_widgetfactory> );

        std::shared_ptr<iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::map<std::string, std::shared_ptr<singlexml_widgetfactory>> factories_;
    };
}

#endif
