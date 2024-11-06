#include "solosnake/blue/blue_codeeditor.hpp"
#include "solosnake/input_event.hpp"
#include "solosnake/vkeys.hpp"
#include <cassert>

using namespace std;
using namespace solosnake;

namespace blue
{
    codeeditor::codeeditor( const std::shared_ptr<compiler>& bcc )
        : text_()
        , bcc_( bcc )
        , modifierFlags_( 0 )
    {
        assert( bcc );
    }

    utf8text codeeditor::text() const
    {
        return utf8text( text_ );
    }

    void codeeditor::on_text_received( const wchar_t c )
    {
        if( c != '\b' )
        {
            text_.append( 1, static_cast<char16_t>( c ) );
        }
    }

    void codeeditor::on_key_pressed( const unsigned short k, const unsigned int modifierFlags )
    {
        switch( k )
        {
            case VK_BACK:
            {
                if( ! text_.empty() )
                {
                    text_.resize( text_.size() - 1 );
                }
            }
            break;

            default:
                break;
        }

        if( input_event::is_alt_down( modifierFlags ) && ! input_event::is_alt_down( modifierFlags_ ) )
        {
            modifierFlags_ |= static_cast<unsigned int>( input_event::alt_down );
        }

        if( input_event::is_shift_down( modifierFlags ) && ! input_event::is_shift_down( modifierFlags_ ) )
        {
            modifierFlags_ |= static_cast<unsigned int>( input_event::shift_down );
        }

        if( input_event::is_ctrl_down( modifierFlags ) && ! input_event::is_ctrl_down( modifierFlags_ ) )
        {
            modifierFlags_ |= static_cast<unsigned int>( input_event::ctrl_down );
        }
    }

    void codeeditor::on_key_released( const unsigned short k, const unsigned int modifierFlags )
    {
        if( input_event::is_alt_down( modifierFlags_ ) && ! input_event::is_alt_down( modifierFlags ) )
        {
            modifierFlags_ &= ~static_cast<unsigned int>( input_event::alt_down );
        }

        if( input_event::is_shift_down( modifierFlags_ ) && ! input_event::is_shift_down( modifierFlags ) )
        {
            modifierFlags_ &= ~static_cast<unsigned int>( input_event::shift_down );
        }

        if( input_event::is_ctrl_down( modifierFlags_ ) && ! input_event::is_ctrl_down( modifierFlags ) )
        {
            modifierFlags_ &= ~static_cast<unsigned int>( input_event::ctrl_down );
        }
    }
}
