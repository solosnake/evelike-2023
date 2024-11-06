#ifndef solosnake_background_widget_hpp
#define solosnake_background_widget_hpp

#include <memory>
#include <string>
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/iwidgetrenderer.hpp"

namespace solosnake
{
    class iwidgetrenderer;

    //! Lua version of a background widget.
    //! Note, a background widget will get focus by default.
    class background_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( background_widget );

        explicit background_widget( lua_State* );

        //! Initially creates a widget with size 1 x 1 but this is resized to
        //! correct screen size by the required "on_screen_resized" call of the
        //! widget system.
        background_widget(
            const std::string& name,
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<iwidgetrenderer>&,
            const ilua_widget::functionnames&,
            iwidget::SharedPtrOnly );

        static std::shared_ptr<background_widget> make_background_widget(
            const std::string& name,
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<iwidgetrenderer>&,
            const ilua_widget::functionnames& );

        void render( const unsigned long dt ) const override;

        //! Background widgets resize themselves to the screen size.
        void on_screen_resized( const dimension2d<unsigned int>& windowSize ) override;

    private:

        std::shared_ptr<iwidgetrenderer> renderer_;
    };
}

#endif
