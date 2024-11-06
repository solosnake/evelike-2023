#ifndef solosnake_textbutton_widget_hpp
#define solosnake_textbutton_widget_hpp

#include <memory>
#include <string>
#include "solosnake/colour.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ishape.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/widgetstate.hpp"
#include "solosnake/widgetlook.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class ilanguagetext;

    //! Exposes simple coloured rectangular button
    //! widget to XML.
    class textbutton_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( textbutton_widget );

        explicit textbutton_widget( lua_State* );

        textbutton_widget(
            const widgetparams&,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        textbutton_widget(
            const widgetparams&,
            std::unique_ptr<ishape>,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        void render( const unsigned long dt ) const override;

        bool is_button_enabled() const;

        void set_button_enabled( bool );

        utf8text get_button_text() const;

        utf8text get_button_text_id() const;

    private:

        WidgetVisualStates get_visual_state( WidgetState ) const;

        int set_screen_quad( lua_State* );

        int get_screen_quad( lua_State* );

        int set_text_id( lua_State* );

        int get_text( lua_State* );

        int get_text_id( lua_State* );

        int set_button_enabled( lua_State* );

    private:

        //! This is not the display text, it is the key to look up the display text
        //! in the language pack.
        utf8text                            text_id_;
        std::shared_ptr<iwidgetrenderer>    style_;
        std::shared_ptr<ilanguagetext>      language_;
        int                                 textsize_;

    private:

        static const widgetlook looks_[2];
    };
}

#endif
