#ifndef blue_gameviewing_widget_hpp
#define blue_gameviewing_widget_hpp

#include <memory>
#include "solosnake/point.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/iwidgetrenderer.hpp"

namespace solosnake
{
    class iwidgetrenderer;
    class luaguicommandexecutor;
}

namespace blue
{
    class iboardview;
    struct Hex_coord;

    //! A base class for using with widgets that want to manipulate a board/camera.
    //! This should be inherited from. By default when the screen is resized this widget's
    //! active area will be resized to match the entire screen, similar to a background
    //! widget.
    class gameviewing_widget : public solosnake::ilua_widget
    {
    public:

        gameviewing_widget(
            const solosnake::widgetparams&,
            const iwidget::KeyboardFocusing,
            const ilua_widget::WidgetDisablePolicy,
            const std::weak_ptr<iboardview>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::iwidgetrenderer>&,
            const solosnake::ilua_widget::functionnames& );

    protected:

        //! Makes the active area the entire screen.
        void on_screen_resized(
            const solosnake::dimension2d<unsigned int>& newWindowSize ) override;

        void on_cursor_enter(
            solosnake::screenxy,
            solosnake::cursorbuttons_state ) override;

        void on_cursor_moved(
            solosnake::screenxy,
            solosnake::cursorbuttons_state ) override;

        void on_cursor_exit(
            solosnake::screenxy,
            solosnake::cursorbuttons_state ) override;

        void on_clicked(
            unsigned short button,
            solosnake::screenxy ) override;

        void on_double_clicked(
            unsigned short button,
            solosnake::screenxy ) override;

        solosnake::ButtonPressOutcome on_button_pressed(
            bool inside,
            solosnake::screenxy,
            unsigned short button ) override;

        solosnake::ButtonReleaseOutcome on_button_released(
            bool inside,
            solosnake::screenxy,
            unsigned short button ) override;

        void on_wheelmoved_inside(
            solosnake::screenxy,
            float ) override;

        void on_wheelmoved_outside(
            solosnake::screenxy,
            float ) override;

        void on_reset() override;

        gameviewing_widget();

        //! Called if the user has clicked on a Hex_coord on the board.
        virtual void on_clicked_on_board( const Hex_coord& );

        std::shared_ptr<iboardview> board_view();

        std::shared_ptr<solosnake::iwidgetrenderer>& renderer();

    private:

        std::shared_ptr<solosnake::iwidgetrenderer> renderer_;
        std::weak_ptr<iboardview>                   boardview_;
        solosnake::screenxy                         previousDrag_;
        bool                                        mouseDragging_;
        bool                                        mouseRotating_;
        solosnake::screenxy                         previousRotate_;
    };


    //////////////////////////////////////////////////////////////////////////


    inline std::shared_ptr<iboardview> gameviewing_widget::board_view()
    {
        return std::shared_ptr<iboardview>( boardview_ );
    }

    inline std::shared_ptr<solosnake::iwidgetrenderer>& gameviewing_widget::renderer()
    {
        return renderer_;
    }
}

#endif
