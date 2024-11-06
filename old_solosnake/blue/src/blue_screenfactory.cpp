#include <cassert>
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/blue/blue_gamescreen.hpp"
#include "solosnake/blue/blue_mapeditscreen.hpp"
#include "solosnake/blue/blue_menuscreen.hpp"
#include "solosnake/blue/blue_screenfactory.hpp"
#include "solosnake/blue/blue_screenxmlnames.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    screenfactory::screenfactory( const shared_ptr<ioswindow>& wndw,
                                  const shared_ptr<user_settings>& settings,
                                  const shared_ptr<translator>& tl8,
                                  const shared_ptr<datapaths>& dataPaths )
        : paths_( dataPaths )
    {
        assert( tl8 );
        assert( settings );
        assert( wndw );

        register_screen_factory( "menu", menuscreen::make_factory( wndw, settings, tl8 ) );

        register_screen_factory( "game", gamescreen::make_factory( wndw, settings, tl8 ) );
        
        register_screen_factory( "mapedit", mapeditscreen::make_factory( wndw, settings, tl8 ) );
    }

    screenfactory::~screenfactory()
    {
    }

    //! Opens the XML file and peeks inside it for the type. This is used to
    //! determine the screen type to create, which will then read the XML again.
    //! Returns an empty string or throws.
    string screenfactory::read_screen_type( const solosnake::nextscreen& nxt ) const
    {
        string screenType;

        if( nxt )
        {
            auto screenFilePath = paths_->get_gui_filepath( nxt.screen_name() );

            TiXmlDocument doc;

            load_xml_doc( doc, screenFilePath.string().c_str() );

            TiXmlNode* pNode = doc.FirstChild( BLUE_SCREEN_XML_ELEMENT );

            if( pNode && pNode->ToElement() )
            {
                const TiXmlElement* const pElem = pNode->ToElement();

                const char* const pScreenType = pElem->Attribute( BLUE_SCREENTYPE_XML_ATTRIBUTE );

                if( pScreenType )
                {
                    screenType = string( pScreenType );
                }
                else
                {
                    ss_err( BLUE_SCREEN_XML_ELEMENT " element is missing attribute "
                            "'" BLUE_SCREENTYPE_XML_ATTRIBUTE "'" );
                }
            }
            else
            {
                ss_throw( "Invalid screen XML file, no '" BLUE_SCREEN_XML_ELEMENT
                          "' element present." );
            }
        }

        return screenType;
    }

    //! Creates a new screen object based on, and using, the given nextscreen
    //! object.
    unique_ptr<iscreen> screenfactory::do_create_screen( const nextscreen& nxt ) const
    {
        assert( nxt );

        auto screenType = read_screen_type( nxt );
        auto factoryItr = factories_.find( screenType );

        if( factoryItr == factories_.end() )
        {
            ss_err( "Unable to find factory matching screen type : ", screenType );
            ss_throw( "Factory could not find requested screen." );
        }

        return factoryItr->second->create_screen( nxt );
    }

    //! Registers the factory to be called when a nextscreen of this name is
    //! passed to create_screen. Throws is the name is an empty string, if the
    //! factory is null, or if there is already a factory registered to this name.
    void screenfactory::register_screen_factory( const string& type,
                                                 const shared_ptr<solosnake::iscreen_factory>& f )
    {
        if( type.empty() )
        {
            ss_throw( "Screen factory requires non empty screen name for registration." );
        }
        else if( !f )
        {
            ss_throw( "Cannot register nullptr as factory." );
        }
        else if( factories_.count( type ) != 0 )
        {
            ss_throw( "An attempt was made to register a factory "
                      "with a name that was already in use." );
        }

        factories_[type] = f;
    }
}
