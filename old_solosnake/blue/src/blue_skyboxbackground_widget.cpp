#include <cassert>
#include <cmath>
#include "solosnake/blue/blue_skyboxbackground_widget.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/external/lua/lunar.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
#   define SOLOSNAKE_SKYBOXBACKGROUNDWIDGET_Z        (-1)
#   define SOLOSNAKE_SKYBOXBACKGROUNDWIDGET_TABORDER (-1)

    //! Initially creates a widget with size 1 x 1 but this is resized to
    //! correct screen size by the required "on_screen_resized" call of the
    //! widget system. The true and the -1 z depth ensures we get focus by
    //! default.
    skyboxbackground_widget::skyboxbackground_widget(
        const string& name,     
        const cubemap& skybox,      
        const bool show_skybox,
        const float axisx,
        const float axisy,
        const float axisz,
        const float radians_per_ms,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<deferred_renderer>& renderer,
        const ilua_widget::functionnames& fnames )
        : ilua_widget( widgetparams( name,
                                     rect( 0, 0, dimension2dui( 1, 1 ) ),
                                     SOLOSNAKE_SKYBOXBACKGROUNDWIDGET_Z,
                                     SOLOSNAKE_SKYBOXBACKGROUNDWIDGET_TABORDER ),
                       iwidget::NoKeyBoardFocus,
                       lce,
                       fnames,
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
        , renderer_( renderer )
        , radians_per_ms_( radians_per_ms )
        , angle_(0.0f)
        , skybox_on_( show_skybox )
    {
        unit_axis_[0] = axisx;
        unit_axis_[1] = axisy;
        unit_axis_[2] = axisz;
        normalise3( unit_axis_ );

        renderer->load_skybox( skybox );
        renderer->enable_skybox( skybox_on_ );

        assert( renderer_ );
        lunar<skyboxbackground_widget>::add_as_global( lce->lua(), this, name.c_str(), false );
        call_init();
    }

    skyboxbackground_widget::~skyboxbackground_widget()
    {
    }

    LUNAR_CLASS_FUNCTIONS( skyboxbackground_widget ) = 
    { 
        LUNAR_FUNCTION_END
    };

    void skyboxbackground_widget::render( const unsigned long ms ) const
    {
        if( ms > 0u )
        {
            angle_ = fmodf( radians_per_ms_ * ms + angle_, SS_TWOPI );
        }

        if( skybox_on_ )
        {
            float m3x3[9];
            load_rotation_3x3_unitaxis_angle( radians(angle_), unit_axis_, m3x3 );
            renderer_->set_skybox_3x3_matrix( m3x3 );
        }
    }

    //! This method is never called.
    skyboxbackground_widget::skyboxbackground_widget( lua_State* )
        : ilua_widget( widgetparams(),
                       iwidget::NoKeyBoardFocus,
                       shared_ptr<luaguicommandexecutor>(),
                       functionnames(),
                       ilua_widget::DefaultEnabled,
                       iwidget::SharedPtrOnly() )
    {
        ss_throw( "Calling creation of skyboxbackground_widget via Lua is not allowed." );
    }

    //! Background widgets resize themselves to the screen size.
    void skyboxbackground_widget::on_screen_resized( const dimension2d<unsigned int>& newWindowSize )
    {
        this->set_activearea_rect( rect( 0, 0, newWindowSize ) );
        ilua_widget::on_screen_resized( newWindowSize );
    }
}
