#include "solosnake/blue/blue_screen.hpp"
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_translator.hpp"
#include "solosnake/blue/blue_user_settings.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    screen::screen( const string& xmlScreenFile,
                    const shared_ptr<user_settings>& userSettings,
                    const shared_ptr<translator>& tl8 )
        : xmlScreenFile_( xmlScreenFile )
        , userSettings_( userSettings )
        , translator_( tl8 )
        , lce_( luaguicommandexecutor::make_shared() )
        , paths_( make_shared<datapaths>( userSettings->data_dirs() ) )
    {
        assert( false == xmlScreenFile.empty() );
        assert( userSettings );
        assert( tl8 );
    }

    screen::~screen()
    {
    }

    lua_State* screen::get_lua()
    {
        return lce_->lua();
    }

    solosnake::filepath screen::get_xml_screen_filepath() const
    {
        return get_datapaths()->get_gui_filepath( xmlScreenFile_ );
    }

    std::shared_ptr<datapaths> screen::get_datapaths() const
    {
        return paths_;
    }

    std::shared_ptr<user_settings> screen::get_user_settings() const
    {
        return userSettings_;
    }

    std::shared_ptr<solosnake::luaguicommandexecutor> screen::get_lce() const
    {
        return lce_;
    }

    std::shared_ptr<solosnake::ilanguagetext> screen::get_language_texts() const
    {
        return translator_;
    }
    
    std::shared_ptr<translator> screen::get_translator() const
    {
        return translator_;
    }
}
