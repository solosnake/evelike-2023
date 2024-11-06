#include "solosnake/iwidget.hpp"
#include "solosnake/ishape.hpp"
#include "solosnake/draginfo.hpp"
#include "solosnake/dragpackage.hpp"
#include "solosnake/layout.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

namespace solosnake
{
    namespace
    {
        //! This is used when the pre-condition to calling is known to be
        //! always true. This is the case in iwidget when the active area
        //! bounding rect is identical to the widget ishape.
        class alwaysinsideshape : public ishape
        {
            bool is_xy_inside_shape( float, float ) const override
            {
                return true;
            }
        };
    }

    iwidget::iwidget( const widgetparams& params,
                      const KeyboardFocusing kbf,
                      const SharedPtrOnly )
        : layout_()
        , shape_( new alwaysinsideshape() )
        , name_( params.name )
        , active_area_( params.active_area )
        , zlayer_( params.zlayer )
        , tab_order_value_( params.tab_order_value )
        , flags_( 0 )
    {
        if( params.name.empty() )
        {
            ss_throw( "Widget names must not be empty." );
        }

        flags_ |= ( kbf == YesKeyBoardFocus ) ? WIDGET_ACCEPTS_KEYBOARD_FOCUS_FLAG : 0;
    }

    iwidget::iwidget( const widgetparams& params,
                      const KeyboardFocusing kbf,
                      std::unique_ptr<ishape> s,
                      const SharedPtrOnly )
        : layout_()
        , shape_( std::move( s ) )
        , name_( params.name )
        , active_area_( params.active_area )
        , zlayer_( params.zlayer )
        , tab_order_value_( params.tab_order_value )
        , flags_( 0 )
    {
        if( nullptr == shape_.get() )
        {
            ss_throw( "Widget requires a valid shape object." );
        }

        if( params.name.empty() )
        {
            ss_throw( "Widget names must not be empty." );
        }

        flags_ |= ( kbf == YesKeyBoardFocus ) ? WIDGET_ACCEPTS_KEYBOARD_FOCUS_FLAG : 0;
    }

    iwidget::~iwidget()
    {
    }

    void iwidget::set_activearea_rect( const rect& r )
    {
        active_area_ = r;

        auto sharedLayout = layout_.lock();

        if( sharedLayout )
        {
            sharedLayout->widget_changed_location( *this );
        }
    }

    void iwidget::set_layout( std::weak_ptr<layout> lyt )
    {
        layout_ = lyt;
    }

    void iwidget::enable_widget()
    {
        flags_ &= ~WIDGET_IS_DISABLED_FLAG;

        auto sharedLayout = layout_.lock();

        if( sharedLayout )
        {
            sharedLayout->widget_changed_enabled( *this );
        }
    }

    void iwidget::disable_widget()
    {
        flags_ |= WIDGET_IS_DISABLED_FLAG;

        auto sharedLayout = layout_.lock();

        if( sharedLayout )
        {
            sharedLayout->widget_changed_enabled( *this );
        }
    }

    void iwidget::keyboard_focus_gained()
    {
        ss_dbg( "Widget ", name_.str(), " gained keyboard focus." );

        assert( accepts_keyboard_focus() );
        assert( is_widget_enabled() );
        assert( false == has_keyboard_focus() );

        flags_ |= WIDGET_HAS_KEYBOARD_FOCUS_FLAG;

        assert( has_keyboard_focus() );

        on_keyboard_focus_gained();
    }

    void iwidget::keyboard_focus_lost()
    {
        ss_dbg( "Widget ", name_.str(), " lost keyboard focus." );

        assert( accepts_keyboard_focus() );
        assert( is_widget_enabled() );
        assert( has_keyboard_focus() );

        flags_ &= ~WIDGET_HAS_KEYBOARD_FOCUS_FLAG;

        assert( false == has_keyboard_focus() );

        on_keyboard_focus_lost();
    }

    void iwidget::screen_resized_to( const dimension2d<unsigned int>& windowSize )
    {
        on_screen_resized( windowSize );
    }

    void iwidget::text_received( const wchar_t w )
    {
        ss_dbg( "Widget wchar_t received ", w );
        on_text_received( w );
    }

    void iwidget::key_pressed( const unsigned short k, const unsigned int modifierFlags )
    {
        ss_dbg( "Key ", k, " pressed on ", name_.str() );

        assert( has_keyboard_focus() );

        if( is_widget_enabled() )
        {
            on_key_pressed( k, modifierFlags );
        }
    }

    void iwidget::key_released( const unsigned short k, const unsigned int modifierFlags )
    {
        ss_dbg( "Key ", k, " released on ", name_.str() );

        assert( has_keyboard_focus() );
        assert( is_widget_enabled() );

        if( is_widget_enabled() )
        {
            on_key_released( k, modifierFlags );
        }
    }

    void iwidget::double_clicked( cursorstate c )
    {
        if( is_widget_enabled() )
        {
            on_double_clicked( c.button_index(), c );
        }
    }

    void iwidget::cursor_entered( screenxy xy, cursorbuttons_state cbs )
    {
        if( is_widget_enabled() )
        {
            on_cursor_enter( xy, cbs );
            flags_ |= WIDGET_MOUSE_IS_INSIDE_FLAG;
        }
    }

    void iwidget::cursor_moved( screenxy xy, cursorbuttons_state cbs )
    {
        if( is_widget_enabled() )
        {
            on_cursor_moved( xy, cbs );
        }
    }

    void iwidget::cursor_exited( screenxy xy, cursorbuttons_state cbs )
    {
        if( is_widget_enabled() )
        {
            on_cursor_exit( xy, cbs );
            flags_ &= ~WIDGET_MOUSE_IS_INSIDE_FLAG;
            assert( 0 == ( flags_ & WIDGET_MOUSE_IS_INSIDE_FLAG ) );
        }
    }

    void iwidget::dragged_over_by( const draginfo&, screenxy, cursorbuttons_state )
    {
    }

    void iwidget::drag_drop_receive( std::unique_ptr<dragpackage>, screenxy )
    {
    }

    std::unique_ptr<draginfo> iwidget::get_drag_info( screenxy ) const
    {
        return std::unique_ptr<draginfo>();
    }

    std::unique_ptr<dragpackage> iwidget::get_drag_package( const draginfo& ) const
    {
        return std::unique_ptr<dragpackage>();
    }

    ButtonPressOutcome iwidget::button_pressed_inside( screenxy xy, cursorbuttons_state cbs )
    {
        ss_dbg( "iwidget::button_pressed_inside" );
        ss_dbg( "button index = ", cbs.button_index() );

        if( is_widget_enabled() )
        {
            ButtonPressOutcome outcome = on_button_pressed( true, xy, cbs.button_index() );

            if( outcome != IgnoreButtonPress && cbs.is_action_set() )
            {
                ss_dbg( "Action set on ", name().c_str() );
#ifndef NDEBUG
                if( 0 != ( flags_ & WIDGET_ACTION_IS_PRESSED ) )
                {
                    ss_err( "0 != (flags_ & WIDGET_ACTION_IS_PRESSED)" );
                }
#endif
                flags_ |= WIDGET_ACTION_IS_PRESSED;
            }

            return outcome;
        }

        return IgnoreButtonPress;
    }

    ButtonPressOutcome iwidget::button_pressed_outside( screenxy xy, cursorbuttons_state cbs )
    {
        ss_dbg( "iwidget::button_pressed_outside" );
        if( is_widget_enabled() )
        {
            on_button_pressed( false, xy, cbs.button_index() );
        }

        return IgnoreButtonPress;
    }

    ButtonReleaseOutcome iwidget::button_released_inside( screenxy xy, cursorbuttons_state cbs )
    {
        ss_dbg( "iwidget::button_released_inside" );
        if( is_widget_enabled() )
        {
            ss_dbg( "iwidget is enabled" );
            on_button_released( true, xy, cbs.button_index() );

            if( cbs.is_action_set() && is_action_pressed() )
            {
                ss_dbg( "iwidget action is set and pressed" );
                on_clicked( cbs.button_index(), xy );
            }
        }

        // Clear the action-is-pressed-down flag.
        if( cbs.is_action_set() && is_action_pressed() )
        {
            ss_dbg( "Action cleared on ", name().c_str() );
            flags_ &= ~WIDGET_ACTION_IS_PRESSED;
            assert( false == is_action_pressed() );
        }

        return ActionCompleted;
    }

    ButtonReleaseOutcome iwidget::button_released_outside( screenxy xy, cursorbuttons_state cbs )
    {
        auto outcome = on_button_released( false, xy, cbs.button_index() );

        // Clear the action-is-pressed-down flag.
        if( cbs.is_action_set() && is_action_pressed() )
        {
            ss_dbg( "Action cleared on ", name().c_str() );
            flags_ &= ~WIDGET_ACTION_IS_PRESSED;
            assert( false == is_action_pressed() );
        }

        return outcome;
    }

    void iwidget::button_wheelmoved_inside( screenxy xy, float delta )
    {
        on_wheelmoved_inside( xy, delta );
    }

    void iwidget::button_wheelmoved_outside( screenxy xy, float delta )
    {
        on_wheelmoved_outside( xy, delta );
    }

    bool iwidget::contains( int screenx, int screeny ) const
    {
        // The screen x and y coordinates range from 0,0 (top left)
        // to w,h (bottom right).

        if( active_area().contains( screenx, screeny ) )
        {
            // Transform the screen xy into the active area rect
            // coordinate system. This is a normalised coordinate system
            // within which lies a shape.
            const float wx = static_cast<float>( screenx - active_area().left() );
            const float wy = static_cast<float>( screeny - active_area().top() );
            const float ax = wx / active_area().width();
            const float ay = wy / active_area().height();
            assert( wx >= 0.0f );
            assert( wy >= 0.0f );
            assert( ax < 1.0f );
            assert( ay < 1.0f );
            return shape_->is_xy_inside_shape( ax, ay );
        }
        else
        {
            return false;
        }
    }
}
