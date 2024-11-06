#ifndef solosnake_checkbutton_widget_hpp
#define solosnake_checkbutton_widget_hpp

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

    //! A check button widget is a rectangular button with a tickbox to one side
    //! and which can have text upon it.
    class checkbutton_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( checkbutton_widget );

        explicit checkbutton_widget( lua_State* );

        checkbutton_widget(
            const widgetparams&,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const std::string& oncheckchanged_fname,
            const bool ticked,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        checkbutton_widget(
            const widgetparams&,
            std::unique_ptr<ishape>,
            const std::shared_ptr<luaguicommandexecutor>,
            const ilua_widget::functionnames&,
            const std::string& oncheckchanged_fname,
            const bool ticked,
            const utf8text&,
            const int textsize,
            const std::shared_ptr<iwidgetrenderer>,
            const std::shared_ptr<ilanguagetext> );

        void render( const unsigned long dt ) const override;

        bool is_button_enabled() const;

        bool is_button_checked() const;

        void set_button_checked( const bool );

        void toggle_checked();

        void set_button_enabled( const bool );

        utf8text get_button_text() const;

        utf8text get_button_text_id() const;

    private:

        WidgetVisualStates get_visual_state( WidgetState ) const;

        int set_screen_quad( lua_State* );

        int get_screen_quad( lua_State* );

        int set_text_id( lua_State* );

        int get_text( lua_State* );

        int get_text_id( lua_State* );

        int set_button_checked( lua_State* );

        int toggle_checked( lua_State* );

        int is_button_checked( lua_State* );

        int set_button_enabled( lua_State* );

        void on_check_changed();

    private:

        //! This is not the display text, it is the key to look up the display text
        //! in the language pack.
        std::shared_ptr<iwidgetrenderer>    style_;
        std::shared_ptr<ilanguagetext>      language_;
        std::string                         on_check_changed_function_name_;
        utf8text                            text_id_;
        widgetlook                          looks_;
        int                                 textsize_;
        bool                                checked_;

    private:

    };
}

#endif
