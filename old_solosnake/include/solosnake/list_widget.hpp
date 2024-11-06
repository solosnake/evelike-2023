#ifndef solosnake_filelist_widget_hpp
#define solosnake_filelist_widget_hpp

#include <memory>
#include <string>
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class ilanguagetext;
    struct widgetparams;

    //! Exposes a widget that shows a up and down scrollable list of
    //! files in a given folder.
    class list_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( list_widget );

        explicit list_widget( lua_State* );

        list_widget(
            const std::shared_ptr<std::vector<std::string>>&,
            const widgetparams& params,
            const std::shared_ptr<luaguicommandexecutor> lce,
            const ilua_widget::functionnames& fnames,
            const std::shared_ptr<iwidgetrenderer> renderingStyle,
            iwidget::SharedPtrOnly spo );

        static std::shared_ptr<list_widget> make_filelist_widget(
            const std::shared_ptr<std::vector<std::string>>&,
            const widgetparams& params,
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<iwidgetrenderer>&,
            const ilua_widget::functionnames& );

        void render( const unsigned long dt ) const override;

    private:

        std::shared_ptr<std::vector<std::string>>   list_;
        std::shared_ptr<iwidgetrenderer>            style_;
        int                                         textsize_;
    };
}

#endif
