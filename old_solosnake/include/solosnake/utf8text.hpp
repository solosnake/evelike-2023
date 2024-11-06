#ifndef solosnake_utf8text_hpp
#define solosnake_utf8text_hpp

#include <cstdint>
#include <string>
#include <iosfwd>

namespace solosnake
{
    //! Class for conversion of and iterating over UTF-8 character strings.
    //! ASCII text is processed fast and as normal. UTF-8 text is cleaned of
    //! invalid characters, invalid characters are replaced with UTF standard
    //! missing character token.
    //! Allows for iteration over characters as 32-bit codepoints.
    //! Behaves identically to a std::string in most respects.
    class utf8text
    {
    public:

        utf8text();

        explicit utf8text( const std::u16string& );

        explicit utf8text( std::uint32_t );

        utf8text( const std::string& );

        utf8text( const char* );

        bool operator==( const utf8text& ) const;

        bool operator!=( const utf8text& ) const;

        bool operator<( const utf8text& ) const;

        size_t number_of_characters() const
        {
            return size_;
        }

        size_t bytelength() const
        {
            return text_.size();
        }

        std::uint32_t operator[]( size_t n ) const;

        operator const std::string() const
        {
            return text_;
        }

        const char* c_str() const
        {
            return text_.c_str();
        }

        std::u16string to_utf16() const;

        bool is_empty() const;

    private:
        
        std::string text_;
        size_t      size_;
    };

    std::ostream& operator<<( std::ostream&, const utf8text& );
}

#endif
