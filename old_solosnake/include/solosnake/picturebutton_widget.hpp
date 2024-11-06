#ifndef solosnake_picturebuttonwidget_hpp
#define solosnake_picturebuttonwidget_hpp

#include <string>
#include <memory>
#include "solosnake/colour.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/widgetlook.hpp"

namespace solosnake
{
    class ilanguagetext;
    class iwidgetrenderer;

    //! Exposes simple coloured rectangular button widget to XML.
    class picturebutton_widget : public ilua_widget
    {
    public:

        LUNAR_CLASS( picturebutton_widget );

        explicit picturebutton_widget( lua_State* );

        picturebutton_widget( const widgetparams&,
                              const iwidget::KeyboardFocusing,
                              const solosnake::Rectf& texturearea,
                              const std::string& textureName,
                              const std::shared_ptr<luaguicommandexecutor>&,
                              const std::shared_ptr<iwidgetrenderer>&,
                              const ilua_widget::functionnames&,
                              const utf8text&,
                              const int textsize,
                              const std::shared_ptr<ilanguagetext>& );

        void render( const unsigned long dt ) const override;

        bool is_button_enabled() const;

        void set_button_enabled( bool );

        utf8text get_button_text() const;

        utf8text get_button_text_id() const;

    private:

        WidgetVisualStates get_visual_state( WidgetState ) const;

        void load_and_set_texture( const std::string& );

        int load_and_set_texture( lua_State* );

        int set_screen_quad( lua_State* );

        int set_texture_quad( lua_State* );

        int get_screen_quad( lua_State* );

        int get_texture_quad( lua_State* );

        int set_button_enabled( lua_State* );

        int set_text_id( lua_State* );

        int set_text_size( lua_State* );

        int get_text_size( lua_State* );

        int get_text( lua_State* );

        int get_text_id( lua_State* );

    private:

        std::shared_ptr<iwidgetrenderer>    style_;
        std::shared_ptr<ilanguagetext>      language_;
        widgetlook                          looks_;
        texquad                             texcoords_;
        utf8text                            text_id_;
        int                                 textsize_;
        texturehandle_t                     page_;
    };
}

#endif
