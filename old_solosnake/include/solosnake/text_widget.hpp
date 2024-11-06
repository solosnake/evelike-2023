#ifndef solosnake_text_widget_hpp
#define solosnake_text_widget_hpp

#include <memory>
#include <string>
#include "solosnake/colour.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ishape.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/widgetstate.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class ilanguagetext;

    //! Exposes simple coloured rectangular widget with text on it to XML.
    class text_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( text_widget );

        explicit text_widget( lua_State* );

        text_widget(
            const widgetparams&,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        text_widget(
            const widgetparams&,
            std::unique_ptr<ishape>,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        void render( const unsigned long dt ) const override;

        utf8text get_button_text() const;

        utf8text get_button_text_id() const;

    private:
        
        void on_cursor_enter( screenxy, cursorbuttons_state ) override;

        int set_screen_quad( lua_State* );

        int get_screen_quad( lua_State* );

        int set_text_id( lua_State* );

        int get_text( lua_State* );

        int get_text_id( lua_State* );

    private:

        //! This is not the display text, it is the key to look up the display text
        //! in the language pack.
        utf8text                            text_id_;
        std::shared_ptr<iwidgetrenderer>    style_;
        std::shared_ptr<ilanguagetext>      language_;
        int                                 textsize_;
    };
}

#endif
