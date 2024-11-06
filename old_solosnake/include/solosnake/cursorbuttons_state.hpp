#ifndef cursorbuttons_state_hpp
#define cursorbuttons_state_hpp

#include <cassert>
#include <cstdint>

namespace solosnake
{
    // These are deliberately chosen to mirror the Microsoft Windows
    // cursor message flags.

#   define CURSOR_PRIMARY_BUTTON_FLAG   (0x0001u) // The action cursor button is down.
#   define CURSOR_SECONDARY_BUTTON_FLAG (0x0002u) // The context cursor button is down.
#   define CURSOR_SHIFT_FLAG            (0x0004u) // The SHIFT key is down.
#   define CURSOR_CTRL_FLAG             (0x0008u) // The CTRL key is down.
#   define CURSOR_MIDDLE_BUTTON_FLAG    (0x0010u) // The middle cursor button is down.

#   define HAS_FLAG(S, F)               (0u != ((S) & (F)))

    //! Simple, small and cheap flag carrier for cursor (mouse) button state.
    //! Note that the meanings of the flags can be context sensitive, and do
    //! not necessarily mean that the associated buttons are "down". For example
    //! the cursorbuttons_state can be used to send notifications about buttons
    //! that have been released.
    //! This is a badly designed class, and care must be taken when using it and
    //! interpreting its meaning. For example it could in theory contain several
    //! button states pressed at once and some of the calls would return incorrect
    //! or misleading information in this case.
    class cursorbuttons_state
    {
    public:

        static const unsigned int MaxSupportedCursorButtons = 3u;

        cursorbuttons_state() : flags_( 0u )
        {
        }

        explicit cursorbuttons_state( std::uint16_t flags ) : flags_( flags )
        {
            assert( 0u == ( flags & ~( CURSOR_PRIMARY_BUTTON_FLAG   | 
                                       CURSOR_MIDDLE_BUTTON_FLAG    |
                                       CURSOR_SECONDARY_BUTTON_FLAG | 
                                       CURSOR_SHIFT_FLAG            |
                                       CURSOR_CTRL_FLAG ) ) );
        }

        bool is_action_set() const
        {
            return HAS_FLAG( flags_, CURSOR_PRIMARY_BUTTON_FLAG );
        }

        bool is_middle_set() const
        {
            return HAS_FLAG( flags_, CURSOR_MIDDLE_BUTTON_FLAG );
        }

        bool is_context_set() const
        {
            return HAS_FLAG( flags_, CURSOR_SECONDARY_BUTTON_FLAG );
        }

        bool is_shift_set() const
        {
            return HAS_FLAG( flags_, CURSOR_SHIFT_FLAG );
        }

        bool is_ctrl_set() const
        {
            return HAS_FLAG( flags_, CURSOR_CTRL_FLAG );
        }

        bool any_button_is_set() const
        {
            return 0u != ( flags_ & ( CURSOR_PRIMARY_BUTTON_FLAG | 
                                      CURSOR_MIDDLE_BUTTON_FLAG  |
                                      CURSOR_SECONDARY_BUTTON_FLAG ) );
        }

        //! Returns 0, 1 or 2 assuming only one of CURSOR_PRIMARY_BUTTON_FLAG,
        //! CURSOR_MIDDLE_BUTTON_FLAG or CURSOR_SECONDARY_BUTTON_FLAG has been set.
        //! Meaningless to call is there is no button set.
        unsigned short button_index() const
        {
            assert( any_button_is_set() );

            return HAS_FLAG( flags_, CURSOR_MIDDLE_BUTTON_FLAG )
                   ? 2u
                   : ( ( flags_ & ( CURSOR_PRIMARY_BUTTON_FLAG | CURSOR_SECONDARY_BUTTON_FLAG ) ) - 1u );
        }

        bool operator==( const cursorbuttons_state& other ) const
        {
            return flags_ == other.flags_;
        }

        bool operator!=( const cursorbuttons_state& other ) const
        {
            return flags_ != other.flags_;
        }

    private:

        std::uint16_t flags_;
    };


}

#endif
