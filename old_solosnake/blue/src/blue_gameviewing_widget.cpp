#include <cassert>
#include "solosnake/blue/blue_gameviewing_widget.hpp"
#include "solosnake/blue/blue_igameview.hpp"
#include "solosnake/blue/blue_pickresult.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    gameviewing_widget::gameviewing_widget(
        const widgetparams& params,
        const iwidget::KeyboardFocusing kbf,
        const ilua_widget::WidgetDisablePolicy wdp,
        const weak_ptr<iboardview>& bv,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<iwidgetrenderer>& renderer,
        const ilua_widget::functionnames& fnames )
        : ilua_widget( params, kbf, lce, fnames, wdp, iwidget::SharedPtrOnly() )
        , renderer_( renderer )
        , boardview_( bv )
        , previousDrag_( 0 )
        , mouseDragging_( false )
        , mouseRotating_( false )
        , previousRotate_( 0 )
    {
        assert( ! bv.expired() );
        assert( renderer_ );
    }

    gameviewing_widget::gameviewing_widget()
        : ilua_widget( widgetparams(),
                       iwidget::KeyboardFocusing(),
                       shared_ptr<luaguicommandexecutor>(),
                       ilua_widget::functionnames(),
                       ilua_widget::WidgetDisablePolicy(),
                       iwidget::SharedPtrOnly() )
        , renderer_()
        , boardview_()
        , previousDrag_()
        , mouseDragging_()
        , mouseRotating_()
        , previousRotate_()
    {
    }

    //! Background widgets resize themselves to the screen size.
    void gameviewing_widget::on_screen_resized( const dimension2d<unsigned int>& newWindowSize )
    {
        this->set_activearea_rect( rect( 0, 0, newWindowSize ) );
        ilua_widget::on_screen_resized( newWindowSize );
    }

    void gameviewing_widget::on_clicked( unsigned short button, screenxy xy )
    {
        ilua_widget::on_clicked( button, xy );

        // Throws if gameview_ has expired.
        shared_ptr<iboardview> bv( boardview_ );
        pickresult pick = bv->get_boardpick( xy );

        if( pick.onboard )
        {
            ss_dbg( "Clicked on ", static_cast<int>( pick.boardcoord.x ),
                    ' ',           static_cast<int>( pick.boardcoord.y ) );

            this->on_clicked_on_board( pick.boardcoord );
        }
    }

    void gameviewing_widget::on_double_clicked( unsigned short button, screenxy xy )
    {
        ilua_widget::on_double_clicked( button, xy );
    }

    void gameviewing_widget::on_clicked_on_board( const Hex_coord& )
    {
    }

    void gameviewing_widget::on_cursor_enter( screenxy, cursorbuttons_state )
    {
    }

    void gameviewing_widget::on_cursor_exit( screenxy, cursorbuttons_state )
    {
    }

    ButtonPressOutcome gameviewing_widget::on_button_pressed( bool inside,
                                                              screenxy xy,
                                                              unsigned short button )
    {
        if( inside )
        {
            ilua_widget::on_button_pressed( true, xy, button );

            if( button == 0 )
            {
                mouseDragging_ = true;
                previousDrag_ = xy;
            }
            else if( button == 1 )
            {
                mouseRotating_ = true;
                previousRotate_ = xy;
            }

            return ExclusiveButtonPress;
        }

        return IgnoreButtonPress;
    }

    void gameviewing_widget::on_cursor_moved( screenxy xy, cursorbuttons_state )
    {
        if( mouseDragging_ && previousDrag_ != xy )
        {
            // Throws if view has expired.
            shared_ptr<iboardview> bv( boardview_ );

            // Clamp to window bounds: ignore mouse movements outside of the
            // window area.
            xy = xy.clamped(
                     static_cast<int16_t>( active_area().width() ),
                     static_cast<int16_t>( active_area().height() ) );

            const point3d oldPick = bv->get_boardplanepick( previousDrag_ );

            previousDrag_ = xy;

            const point3d newPick = bv->get_boardplanepick( xy );

            point3d delta;
            sub3( oldPick.xyz, newPick.xyz, delta.xyz );

            bv->translate_camera( delta.xyz[0], delta.xyz[2] );
        }

        if( mouseRotating_ && previousRotate_ != xy )
        {
            // Throws if gameview_ has expired.
            shared_ptr<iboardview> bv( boardview_ );

            // Clamp to window bounds: ignore mouse movements outside of the
            // window area.
            xy = xy.clamped(
                     static_cast<int16_t>( active_area().width() ),
                     static_cast<int16_t>( active_area().height() ) );

            const auto previousRotate = previousRotate_;
            previousRotate_ = xy;

            const point3d lookedAt = bv->camera_lookedat();
            const point3d oldPick  = bv->get_boardplanepick( previousRotate );
            const point3d newPick  = bv->get_boardplanepick( xy );

            // Find the angle between the three points.
            point3d rotateFrom;
            point3d rotateTo;
            sub3( oldPick.xyz, lookedAt.xyz, rotateFrom.xyz );
            sub3( newPick.xyz, lookedAt.xyz, rotateTo.xyz );

            // 2D Y=0 plane problem:
            const float angleFrom = atan2( rotateFrom.xyz[0], rotateFrom.xyz[2] );
            const float angleTo = atan2( rotateTo.xyz[0], rotateTo.xyz[2] );

            bv->rotate_camera( radians( angleTo - angleFrom ) );
        }
    }

    ButtonReleaseOutcome gameviewing_widget::on_button_released( bool inside,
                                                                 screenxy xy,
                                                                 unsigned short button )
    {
        if( button == 0 )
        {
            mouseDragging_ = false;
        }

        if( button == 1 )
        {
            mouseRotating_ = false;
        }

        if( false == inside )
        {
            ilua_widget::on_button_released( false, xy, button );
            mouseDragging_ = false;
            mouseRotating_ = false;
        }

        return ActionCompleted;
    }

    void gameviewing_widget::on_wheelmoved_inside( screenxy xy, float d )
    {
        ilua_widget::on_wheelmoved_inside( xy, d );
        shared_ptr<iboardview>( boardview_ )->move_camera_forwards( d );
    }

    void gameviewing_widget::on_wheelmoved_outside( screenxy xy, float d )
    {
        ilua_widget::on_wheelmoved_outside( xy, d );
        shared_ptr<iboardview>( boardview_ )->move_camera_forwards( d );
    }

    void gameviewing_widget::on_reset()
    {
        ilua_widget::on_reset();
        mouseDragging_ = false;
        mouseRotating_ = false;
    }
}
