#ifndef blue_codeeditor_widget_hpp
#define blue_codeeditor_widget_hpp

#include <memory>
#include "solosnake/ilua_widget.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/widgetvisualstates.hpp"
#include "solosnake/blue/blue_codeeditor.hpp"
#include "solosnake/blue/blue_compiler.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class luaguicommandexecutor;
}

namespace blue 
{
    //! A widget for editing code scripts.
    class codeeditor_widget : public solosnake::ilua_widget
    {
    public:

        LUNAR_CLASS( codeeditor_widget );

        codeeditor_widget( lua_State* );

        codeeditor_widget(
            const solosnake::widgetparams& params,
            const std::shared_ptr<compiler>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>,
            const std::shared_ptr<solosnake::iwidgetrenderer>&,
            const solosnake::ilua_widget::functionnames&,
            const int textsize);

    private:
        
        void render( const unsigned long dt ) const override;
        
        void on_text_received( wchar_t ) override;

        void on_key_pressed( unsigned short k, unsigned int modifierFlags ) override;

        void on_key_released( unsigned short k, unsigned int modifierFlags ) override;
        
        int set_screen_quad( lua_State* L );

    private:
                
        std::shared_ptr<solosnake::iwidgetrenderer> renderer_;
        std::unique_ptr<codeeditor>                 editor_;
        solosnake::WidgetVisualStates               visual_state_;
        int                                         textsize_;
    };
}

#endif
