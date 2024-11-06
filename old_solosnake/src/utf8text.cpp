#include <cassert>
#include <cstring>
#include <fstream>
#include "solosnake/utf8text.hpp"
#include "solosnake/external/utf8.hpp"

namespace solosnake
{
    namespace
    {
        inline std::uint32_t codepoint_at( const char* begin, const char* end, size_t n )
        {
            auto i = begin;
#ifndef NDEBUG
            utf8::advance( i, n, end );
#else
            utf8::unchecked::advance( i, n );
#endif
            return *( utf8::iterator<const char*>( i, begin, end ) );
        }
    }

    std::ostream& operator<<( std::ostream& os, const utf8text& txt )
    {
        return os << static_cast<const std::string&>( txt );
    }

    std::uint32_t utf8text::operator[]( size_t n ) const
    {
        assert( !text_.empty() );
        return size_ == text_.size() ? static_cast<std::uint32_t>( text_[n] )
               : codepoint_at( &text_[0], &text_[0] + text_.size(), n );
    }

    utf8text::utf8text() : size_( 0 )
    {
    }

    utf8text::utf8text( const std::u16string& u16 )
    {
        utf8::utf16to8( u16.cbegin(), u16.cend(), back_inserter( text_ ) );
        size_ = text_.empty() ? 0 : utf8::distance( &text_[0], &text_[0] + text_.size() );
    }

    utf8text::utf8text( std::uint32_t codepoint32 )
    {
        std::uint32_t text32[2] = { codepoint32, 0 };
        utf8::utf32to8( text32, text32 + 1, std::back_inserter( text_ ) );
        size_ = text_.empty() ? 0 : utf8::distance( &text_[0], &text_[0] + text_.size() );
    }

    utf8text::utf8text( const std::string& txt ) : text_( txt )
    {
        assert( !utf8::is_bom( txt.c_str() ) );

        std::string temp;
        temp.reserve( text_.size() );
        utf8::replace_invalid( text_.cbegin(), text_.cend(), std::back_inserter( temp ) );
        text_.swap( temp );
        size_ = text_.empty() ? 0 : utf8::distance( &text_[0], &text_[0] + text_.size() );
    }

    utf8text::utf8text( const char* txt ) : text_( txt )
    {
        assert( txt );
        assert( !utf8::is_bom( txt ) );

        std::string temp;
        temp.reserve( text_.size() );
        utf8::replace_invalid( text_.cbegin(), text_.cend(), std::back_inserter( temp ) );
        text_.swap( temp );
        size_ = text_.empty() ? 0 : utf8::distance( &text_[0], &text_[0] + text_.size() );
    }

    bool utf8text::operator==( const utf8text& rhs ) const
    {
        return text_ == rhs.text_;
    }

    bool utf8text::operator!=( const utf8text& rhs ) const
    {
        return text_ != rhs.text_;
    }

    bool utf8text::operator<( const utf8text& rhs ) const
    {
        return text_ < rhs.text_;
    }

    std::u16string utf8text::to_utf16() const
    {
        std::u16string utf16;
        auto end_it = utf8::find_invalid( text_.cbegin(), text_.cend() );
        utf8::utf8to16( text_.cbegin(), end_it, std::back_inserter( utf16 ) );
        return utf16;
    }

    bool utf8text::is_empty() const
    {
        return size_ == 0;
    }
}