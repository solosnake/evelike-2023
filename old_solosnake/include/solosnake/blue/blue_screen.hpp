#ifndef blue_screen_hpp
#define blue_screen_hpp

#include <memory>
#include <string>
#include "solosnake/filepath.hpp"
#include "solosnake/ilanguagetext.hpp"
#include "solosnake/iscreen.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/external/lua.hpp"

namespace blue
{
    class datapaths;
    class translator;
    class user_settings;
    class widgetsfactory;

    //! Base class for blue screens. A screen is a mini app that is created and displayed, and 
    //! which upon exiting tells the app which screen should be displayed next. A screen has a 
    //! 'type', and an XML file specifying how that type should be created. The types of widgets 
    //! that are able to be placed on a screen depends on the type of the screen.
    class screen : public solosnake::iscreen
    {
    public:

        virtual ~screen();

    protected:

        screen(            
            const std::string& xmlFile,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>& );

        //! Allows inheriting classes to access the Lua context.
        lua_State* get_lua();

        //! Allows inheriting classes to access the xml screen file for this screen.
        solosnake::filepath get_xml_screen_filepath() const;

        //! Allows inheriting classes to access the application data paths.
        std::shared_ptr<datapaths> get_datapaths() const;

        //! Allows inheriting classes to access the user settings.
        std::shared_ptr<user_settings> get_user_settings() const;

        //! Allows inheriting classes to access the Lua command executor for this screen.
        std::shared_ptr<solosnake::luaguicommandexecutor> get_lce() const;

        //! Allows inheriting classes to access the language texts.
        std::shared_ptr<solosnake::ilanguagetext> get_language_texts() const;
        
        //! Allows inheriting classes to access the translator.
        std::shared_ptr<translator> get_translator() const;

    private:

        std::string                                         xmlScreenFile_;
        std::shared_ptr<user_settings>                      userSettings_;
        std::shared_ptr<translator>                         translator_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<datapaths>                          paths_;
    };
}

#endif
