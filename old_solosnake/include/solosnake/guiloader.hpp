#ifndef solosnake_guiloader_hpp
#define solosnake_guiloader_hpp

#include <list>
#include <memory>
#include <string>
#include "solosnake/ixml_widgetfactory.hpp"
#include "solosnake/ixmlelementreader.hpp"

namespace solosnake
{
    //! An XML loader for the gui class. Parses the XML file
    //! for a 'widgets' block and creates a set of widgets
    //! based on the XML contents, using the provided factory
    //! set. The loader is not fault tolerant and expects
    //! to be able to construct all widget types found in the
    //! file.
    class guiloader : public ixmlelementreader
    {
    public:

        guiloader( const std::shared_ptr<ixml_widgetfactory>& maker );

        void move_widgets_to( std::list<std::shared_ptr<iwidget>>& other );

        virtual ~guiloader();

    private:

        bool can_read_version( const std::string& ) const override;

        bool read_from_element( const TiXmlElement& ) override;

    private:
        std::string                         widget_;
        std::string                         widgetType_;
        std::list<std::shared_ptr<iwidget>> widgets_;
        std::shared_ptr<ixml_widgetfactory> widgetMaker_;
    };
}

#endif
