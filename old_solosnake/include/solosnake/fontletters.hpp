#ifndef solosnake_fontletters_hpp
#define solosnake_fontletters_hpp

#include <map>
#include <memory>
#include <cstdint>
#include <string>
#include <iosfwd>
#include "solosnake/filepath.hpp"
#include "solosnake/fontglyph.hpp"

namespace solosnake
{
    //! Maps a set of uint32 text codepoints to sub images on texture sheets.
    //! Knows the name of the image (texture page) to use to render the letters.
    class fontletters
    {
    public:

        explicit fontletters( const filepath& fontfile );

        fontletters( const std::string& textureFilename,
                     const std::map<std::uint32_t, fontglyph>& codepoints,
                     const fontglyph& unknownLetter,
                     const unsigned int lineSpacing,
                     const unsigned int whitespaceSize );

        fontletters( const fontletters& );

        fontletters( fontletters&& );

        fontletters& operator=( const fontletters& );

        fontletters& operator=( fontletters && );

        virtual ~fontletters();

        bool save( std::ostream& ) const;

        bool save( const char* filename ) const;

        const fontglyph& get_glyph( std::uint32_t codepoint ) const;

        unsigned int line_spacing() const;

        unsigned int whitespace_size() const;

        const std::string& texture_filename() const;

        size_t letters_count() const;

        bool operator==( const fontletters& ) const;

        bool operator!=( const fontletters& ) const;

    private:
        std::string                         textureFilename_;
        std::map<std::uint32_t, fontglyph>  codepoints_;
        fontglyph                           unknown_letter_;
        unsigned int                        line_spacing_;
        unsigned int                        whitespace_size_;
    };


    //-------------------------------------------------------------------------

    inline unsigned int fontletters::line_spacing() const
    {
        return line_spacing_;
    }

    inline unsigned int fontletters::whitespace_size() const
    {
        return whitespace_size_;
    }

    inline const std::string& fontletters::texture_filename() const
    {
        return textureFilename_;
    }

    inline size_t fontletters::letters_count() const
    {
        return codepoints_.size();
    }
}

#endif
