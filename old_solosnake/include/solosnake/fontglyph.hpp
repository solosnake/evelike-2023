#ifndef solosnake_fontglyph_hpp
#define solosnake_fontglyph_hpp

#include <cstdint>
#include "solosnake/intrinsics.hpp"
#include "solosnake/texquad.hpp"

namespace solosnake
{
    //! A description of a font glyph (a letter drawn onscreen): its texture
    //! coordinates, how far to advance the insert position after rendering it,
    //! and its Y offset to use when rendering to the screen.
    //! It does not know which codepoint it represents.
    //! All the measurements are given in pixels, and these are scaled to
    //! screen units during rendering by the font printer, which will know
    //! the screen pixel size.
    class fontglyph
    {
    public:

        fontglyph();

        fontglyph( const texquad& texCoords,
                   unsigned int pixelWidth,
                   unsigned int pixelHeight,
                   unsigned int pixelAdvanceX,
                   int pixelOffsetY );

        //! How many pixels to advance the letter insert position after
        //! drawing the letter.
        unsigned int pixel_advanceX() const;

        //! How many pixels upwards the insert position should be adjusted
        //! for drawing this letter.
        int pixel_offsetY() const;

        //! Always positive, the width of the characters printed image. This is
        //! effectively the width of the texture coordinate quad.
        unsigned int letter_pixel_width() const;

        //! Always positive, the height of the characters printed image in
        //! pixels. This is effectively the height of the texture coordinate
        //! quad.
        unsigned int letter_pixel_height() const;

        //! The texture coordinates of this letter's image.
        const texquad& textureCoords() const;

        //! Returns the pixel width/height/advance/offset as floats inside
        //! a SIMD tuple 0,1,2,3.
        __m128 get_pixel_w_h_a_o() const;

        bool operator==( const fontglyph& ) const;
        bool operator!=( const fontglyph& ) const;

    private:
        texquad         texCoords_;
        unsigned int    pixelWidth_;
        unsigned int    pixelHeight_;
        unsigned int    pixelAdvanceX_;
        int             pixelOffsetY_;
    };

    //-------------------------------------------------------------------------

    inline unsigned int fontglyph::pixel_advanceX() const
    {
        return pixelAdvanceX_;
    }

    inline int fontglyph::pixel_offsetY() const
    {
        return pixelOffsetY_;
    }

    inline const texquad& fontglyph::textureCoords() const
    {
        return texCoords_;
    }

    inline unsigned int fontglyph::letter_pixel_width() const
    {
        return pixelWidth_;
    }

    inline unsigned int fontglyph::letter_pixel_height() const
    {
        return pixelHeight_;
    }

    inline __m128 fontglyph::get_pixel_w_h_a_o() const
    {
        return _mm_set_ps( static_cast<float>( pixelOffsetY_ ),
                           static_cast<float>( pixelAdvanceX_ ),
                           static_cast<float>( pixelHeight_ ),
                           static_cast<float>( pixelWidth_ ) );
    }

    inline bool fontglyph::operator!=( const fontglyph& rhs ) const
    {
        return !( ( *this ) == rhs );
    }
}

#endif
