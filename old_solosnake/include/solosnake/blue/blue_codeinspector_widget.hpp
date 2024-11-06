#ifndef blue_codeinspectorwidget_hpp
#define blue_codeinspectorwidget_hpp

#include <string>
#include <memory>
#include "solosnake/point.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/widgetvisualstates.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class luaguicommandexecutor;
}

namespace blue
{
    class igameview;
    class Machine;
    class player;
    class compiler;

    //! Widget which displays the current set of instructions for a machine
    class codeinspector_widget : public solosnake::ilua_widget
    {
    public:

        LUNAR_CLASS( codeinspector_widget );

        codeinspector_widget( lua_State* );

        codeinspector_widget(
            const solosnake::widgetparams& params,
            const std::shared_ptr<player>&,
            const std::shared_ptr<compiler>&,
            const std::weak_ptr<igameview>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>,
            const solosnake::ilua_widget::functionnames&,
            const int textsize,
            const std::shared_ptr<solosnake::iwidgetrenderer>& );

        virtual ~codeinspector_widget();

    private:

        int set_screen_quad( lua_State* L );

        void render( const unsigned long dt ) const override;

        void on_double_clicked( unsigned short b, solosnake::screenxy ) override;

        void advance_one_frame() override;

        void update_code();

    private:

        std::shared_ptr<solosnake::iwidgetrenderer> renderer_;
        std::shared_ptr<player>                     player_;
        std::shared_ptr<compiler>                   compiler_;
        solosnake::utf8text                         code_as_text_;
        std::weak_ptr<igameview>                    gameview_;
        const Machine*                              watched_machine_;
        solosnake::WidgetVisualStates               visual_state_;
        int                                         textsize_;
    };
}

#endif
