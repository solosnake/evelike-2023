#ifndef solosnake_cursorstate_hpp
#define solosnake_cursorstate_hpp

#include <cassert>
#include "solosnake/screenxy.hpp"
#include "solosnake/cursorbuttons_state.hpp"

namespace solosnake
{
    //! Mouse events typically need to transmit the location and the
    //! mouse button state. This is just a simple class for this,
    //! containing two other classes.
    class cursorstate
    {
    public:

        cursorstate(
            screenxy xy,
            cursorbuttons_state s ) : xy_( xy ), cursorstate_( s )
        {
            static_assert( sizeof( solosnake::cursorstate ) <= 8, "cursorstate has an unexpected size." );
        }

        cursorstate(
            std::int16_t x,
            std::int16_t y,
            cursorbuttons_state s ) : xy_( x, y ), cursorstate_( s )
        {
        }

        operator const screenxy& () const
        {
            return xy_;
        }

        operator screenxy& ()
        {
            return xy_;
        }

        operator const cursorbuttons_state& () const
        {
            return cursorstate_;
        }

        operator cursorbuttons_state& ()
        {
            return cursorstate_;
        }

        //! The coordinate is relative to the upper-left corner of the client area.
        int screen_x() const
        {
            return xy_.screen_x();
        }

        //! The coordinate is relative to the upper-left corner of the client area.
        int screen_y() const
        {
            return xy_.screen_y();
        }

        bool is_action_set() const
        {
            return cursorstate_.is_action_set();
        }

        bool is_middle_set() const
        {
            return cursorstate_.is_middle_set();
        }

        bool is_context_set() const
        {
            return cursorstate_.is_context_set();
        }

        bool is_shift_set() const
        {
            return cursorstate_.is_shift_set();
        }

        bool is_ctrl_set() const
        {
            return cursorstate_.is_ctrl_set();
        }

        bool any_button_is_set() const
        {
            return cursorstate_.any_button_is_set();
        }

        unsigned short button_index() const
        {
            return cursorstate_.button_index();
        }

    private:
        screenxy xy_;
        cursorbuttons_state cursorstate_;
    };
}

#endif
