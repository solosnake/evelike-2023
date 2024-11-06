#ifndef screenfactory_hpp
#define screenfactory_hpp

#include <map>
#include <memory>
#include "solosnake/ilanguagetext.hpp"
#include "solosnake/iscreen_factory.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/blue/blue_datapaths.hpp"

namespace blue
{
    class user_settings;
    class translator;

    //! A screen is the top level unit of GUI application. The application   //
    //! moves from screen to screen, each screen containing different        //
    //! contents, views onto that contents, and widgets dealing with that    //
    //! contents.                                                            //
    //!                                                                      //
    //!                                                                      //
    //! Creates the screens for the blue application.                        //
    //! A screen in XML has the following format:                            //
    //! \code                                                                //
    //! <screen type="ScreenType">                                           //
    //!     <view name="View1Name">                                          //
    //!     </view>                                                          //
    //!         <gui>                                                        //
    //!             <!-- Widgets are optional - warning                      //
    //!             will be issued if they are missing -->                   //
    //!             <widget name="Widget1Name" type="Widget1Type" ... >      //
    //!                 <!-- widget type specific elements here -->          //
    //!             </widget>                                                //
    //!             <!-- Optional elements here -->                          //
    //!         </gui>                                                       //
    //!     <view name="View2Name">                                          //
    //!         <gui>                                                        //
    //!         </gui>                                                       //
    //!     </view>                                                          //
    //! </screen>                                                            //
    //! \endcode                                                             //
    //!                                                                      //
    //! Each screen has a Lua instance associated with it and all            //
    //! elements of the screen can theoretically share this Lua              //
    //! environment.                                                         //
    //!                                                                      //
    class screenfactory : public solosnake::iscreen_factory
    {
    public:

        screenfactory(
            const std::shared_ptr<solosnake::ioswindow>&,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<translator>&,
            const std::shared_ptr<datapaths>& );

        virtual ~screenfactory();

    private:
        
        std::unique_ptr<solosnake::iscreen> 
        do_create_screen( const solosnake::nextscreen& ) const override;

        std::string read_screen_type(
            const solosnake::nextscreen& ) const;

        void register_screen_factory(
            const std::string& type,
            const std::shared_ptr<solosnake::iscreen_factory>& );

        typedef std::map<std::string, std::shared_ptr<solosnake::iscreen_factory>> factory_map_t;

        factory_map_t                       factories_;
        std::shared_ptr<datapaths>          paths_;
        std::shared_ptr<user_settings>      user_settings_;
    };
}

#endif
