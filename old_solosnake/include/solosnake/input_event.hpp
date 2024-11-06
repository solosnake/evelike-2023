#ifndef solosnake_input_event_hpp
#define solosnake_input_event_hpp

#include <cstdint>
#include <cassert>
#include "solosnake/screenxy.hpp"
#include "solosnake/cursorbuttons_state.hpp"

#ifdef _MSC_VER
#if _MSC_VER <= 1600
//      error C4480: nonstandard extension used: specifying underlying type for
// enum 'solosnake::event_sender_type'
#pragma warning(push)
#pragma warning(disable : 4480)
#endif
#endif

#define SS_SIGNED_TO_UNSIGNED(X) (*reinterpret_cast<const unsigned short*>(&X))
#define SS_UNSIGNED_TO_SIGNED(X) (*reinterpret_cast<const short*>(&X))
#define SS_WHEEL_STEPSIZE (120)

namespace solosnake
{
    //! The API defines 3 Cursor input button events, which are analogous to
    //! the left, middle and right cursor button presses and double clicks
    //! but which may be generated from other sources.
    //! It also defines 2 modifiers for the buttons, SHIFT and CTRL.
    class input_event
    {
    public:
        //! Enumeration of the different kinds of input sources.
        //! This is used to distinguish the events from each other and allow
        //! understanding of the meanings of the other data elements of the
        //! input_event object.
        enum sender_type : std::uint8_t
        {
            unknown_sender_type,
            cursor_sender,
            keyboard_sender,
            joystick_sender,
            focus_sender
        };

        //! There is a limited range of input events that can be generated,
        //! generally speaking button and cursor clicks, and cursor moves and
        //! axis changes (axis are not handled yet).
        enum activity_type : std::uint8_t
        {
            nothing_event,
            button_down_event,
            button_up_event,
            button_dbl_clicked_event,
            key_up_event,
            key_down_event,
            cursor_moved_event,
            text_event,
            focus_lost_event,
            focus_gained_event,
            wheel_moved
        };

        enum modifier_type : std::uint32_t
        {
            alt_down   = 0x1u,
            ctrl_down  = 0x2u,
            shift_down = 0x4u
        };

        input_event()
            : event_activity_type_( nothing_event )
            , event_sender_type_( unknown_sender_type )
            , event_sender_( 0 )
            , event_data_( 0 )
        {
        }

        input_event( std::uint8_t t, std::uint8_t a, std::uint16_t sender, std::uint32_t data )
            : event_activity_type_( a ), event_sender_type_( t ), event_sender_( sender ), event_data_( data )
        {
        }

        cursorbuttons_state cursor() const;

        screenxy cursor_move_xy() const;

        static input_event make_focus_lost();

        static input_event make_focus_gained();

        static input_event make_wheelmove( std::int16_t wheelDelta, screenxy xy );

        static input_event make_cursor_button_down( std::uint16_t buttonFlag, screenxy xy );

        static input_event make_cursor_button_up( std::uint16_t buttonFlag, screenxy xy );

        static input_event make_cursor_dbl_click( std::uint16_t buttonFlag, screenxy xy );

        static input_event make_cursor_move( std::uint16_t cursorFlags, screenxy xy );

        static input_event make_key_down( std::uint16_t keycode, std::uint32_t keymodifiers );

        static input_event make_key_up( std::uint16_t keycode, std::uint32_t keymodifiers );

        static input_event make_text_entry( wchar_t txt );

        static bool is_alt_down( const std::uint32_t keymodifiers );

        static bool is_ctrl_down( const std::uint32_t keymodifiers );

        static bool is_shift_down( const std::uint32_t keymodifiers );

        std::uint8_t event_type() const
        {
            return event_activity_type_;
        }

        std::uint8_t sender_type() const
        {
            return event_sender_type_;
        }

        //! Positive means the user moved the wheel forwards, negative means
        //! they rolled it backwards. 120 means one whole wheel "step".
        //! Partial steps are possible (< 120).
        std::int16_t wheeldelta() const;

        std::uint16_t vkey() const;

        std::uint32_t key_modifiers() const;

        wchar_t text_character() const;

    private:
        std::uint8_t  event_activity_type_;
        std::uint8_t  event_sender_type_;
        std::uint16_t event_sender_;
        std::uint32_t event_data_;
    };

    // Build and interpret event data.

    //! Positive means the user moved the wheel forwards, negative means they
    //! rolled it backwards. 120 means one whole wheel "step". Partial steps
    //! are possible (< 120).
    inline std::int16_t input_event::wheeldelta() const
    {
        assert( sender_type() == static_cast<std::uint8_t>( input_event::cursor_sender ) );
        return SS_UNSIGNED_TO_SIGNED( event_sender_ );
    }

    inline std::uint16_t input_event::vkey() const
    {
        assert( sender_type() == static_cast<std::uint8_t>( input_event::keyboard_sender ) );
        return event_sender_;
    }

    inline std::uint32_t input_event::key_modifiers() const
    {
        assert( sender_type() == static_cast<std::uint8_t>( input_event::keyboard_sender ) );
        return event_data_;
    }

    inline wchar_t input_event::text_character() const
    {
        assert( sender_type() == static_cast<std::uint8_t>( input_event::keyboard_sender )
                && static_cast<std::uint32_t>( static_cast<wchar_t>( event_data_ ) ) == event_data_ );
        return static_cast<wchar_t>( event_data_ );
    }

    inline input_event input_event::make_focus_lost()
    {
        return input_event( input_event::focus_sender, input_event::focus_lost_event, 0, 0 );
    }

    inline input_event input_event::make_focus_gained()
    {
        return input_event( input_event::focus_sender, input_event::focus_gained_event, 0, 0 );
    }

    inline input_event input_event::make_wheelmove( std::int16_t wheelDelta, screenxy xy )
    {
        return input_event( input_event::cursor_sender,
                            input_event::wheel_moved,
                            SS_SIGNED_TO_UNSIGNED( wheelDelta ),
                            xy.screen_xy() );
    }

    inline input_event input_event::make_cursor_button_down( std::uint16_t buttonFlag, screenxy xy )
    {
        return input_event(
                   input_event::cursor_sender, input_event::button_down_event, buttonFlag, xy.screen_xy() );
    }

    inline input_event input_event::make_cursor_button_up( std::uint16_t buttonFlag, screenxy xy )
    {
        return input_event(
                   input_event::cursor_sender, input_event::button_up_event, buttonFlag, xy.screen_xy() );
    }

    inline input_event input_event::make_cursor_dbl_click( std::uint16_t buttonFlag, screenxy xy )
    {
        return input_event( input_event::cursor_sender,
                            input_event::button_dbl_clicked_event,
                            buttonFlag,
                            xy.screen_xy() );
    }

    inline input_event input_event::make_cursor_move( std::uint16_t cursorFlags, screenxy xy )
    {
        return input_event(
                   input_event::cursor_sender, input_event::cursor_moved_event, cursorFlags, xy.screen_xy() );
    }

    inline input_event input_event::make_key_down( std::uint16_t keycode, std::uint32_t keymodifiers )
    {
        return input_event(
                   input_event::keyboard_sender, input_event::key_down_event, keycode, keymodifiers );
    }

    inline input_event input_event::make_key_up( std::uint16_t keycode, std::uint32_t keymodifiers )
    {
        return input_event(
                   input_event::keyboard_sender, input_event::key_up_event, keycode, keymodifiers );
    }

    inline input_event input_event::make_text_entry( wchar_t txt )
    {
        return input_event( input_event::keyboard_sender, input_event::text_event, 0, txt );
    }

    inline cursorbuttons_state input_event::cursor() const
    {
        return cursorbuttons_state( event_sender_ );
    }

    inline screenxy input_event::cursor_move_xy() const
    {
        // Check this event has an XY in it:
        assert( event_activity_type_ == static_cast<std::uint8_t>( input_event::cursor_moved_event ) ||
                event_activity_type_ == static_cast<std::uint8_t>( input_event::button_down_event ) ||
                event_activity_type_ == static_cast<std::uint8_t>( input_event::button_up_event ) ||
                event_activity_type_ == static_cast<std::uint8_t>( input_event::button_dbl_clicked_event ) ||
                event_activity_type_ == static_cast<std::uint8_t>( input_event::wheel_moved ) );

        assert( event_sender_type_ == static_cast<std::uint8_t>( input_event::cursor_sender ) );

        return screenxy( event_data_ );
    }

    inline bool input_event::is_alt_down( const std::uint32_t keymodifiers )
    {
        return 0 != ( keymodifiers & alt_down );
    }

    inline bool input_event::is_ctrl_down( const std::uint32_t keymodifiers )
    {
        return 0 != ( keymodifiers & ctrl_down );
    }

    inline bool input_event::is_shift_down( const std::uint32_t keymodifiers )
    {
        return 0 != ( keymodifiers & shift_down );
    }
}

#ifdef _MSC_VER
#if _MSC_VER <= 1600
#pragma warning(pop)
#endif
#endif

#endif
