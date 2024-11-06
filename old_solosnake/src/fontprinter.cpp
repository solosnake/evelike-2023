#include <cassert>
#include <cmath>
#include <limits>
#include <xmmintrin.h>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/font.hpp"
#include "solosnake/fontprinter.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/utf8text.hpp"
#include "solosnake/window.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        //! Same as ANSI C isspace(int)
        inline bool is_whitespace( uint32_t letter )
        {
            return letter == ' '  || // (0x20) space (SPC)
                   letter == '\t' || // (0x09) horizontal tab (TAB)
                   letter == '\n' || // (0x0a) newline (LF)
                   letter == '\v' || // (0x0b) vertical tab (VT)
                   letter == '\f' || // (0x0c) feed (FF)
                   letter == '\r';   // (0x0d) carriage return (CR)
        }

        inline bool is_linebreak( uint32_t letter )
        {
            return letter == '\n' || letter == '\r';
        }

        inline bool is_not_whitespace( uint32_t letter )
        {
            return false == is_whitespace( letter );
        }

        //! Returns index of first non whitespace codepoint, or txt.size() if
        //! all are whitespace or txt is empty.
        size_t find_first_non_whitespace( const utf8text& txt, size_t startingAt = 0 )
        {
            const size_t n = txt.number_of_characters();

            for( size_t i = startingAt; i < n; ++i )
            {
                if( is_not_whitespace( txt[i] ) )
                {
                    return i;
                }
            }

            return n;
        }
    }

    fontprinter::fontprinter( const dimension2d<unsigned int>& screenSize,
                              const shared_ptr<deferred_renderer>& rndr,
                              const shared_ptr<font>& f )
        : renderer_( rndr ), fonttexture_( 0 ), fontcolour_( 0xFF, 0xFF, 0xFF, 0xFF )
    {
        letters_.reserve( 128 );
        assert( f );

        set_screen_size( screenSize );

        set_font( f );
    }

    void fontprinter::set_font( const shared_ptr<font>& f )
    {
        assert( f );

        if( f != font_ )
        {
            font_ = f;
            fonttexture_ = f->texhandle();
            lineheight_ = static_cast<float>( f->line_spacing() );
        }
    }

    //! Updates any dependencies on screen size. It is up to the user to maintain
    //! the correct screen size.
    void fontprinter::set_screen_size( const dimension2d<unsigned int>& screenSize )
    {
        screenSize_ = screenSize;
    }

    void fontprinter::centre_align_line( const size_t linestart,
                                         const size_t lineend,
                                         const float centreX )
    {
        assert( linestart < letters_.size() );
        assert( lineend <= letters_.size() );
        assert( linestart <= lineend );

        if( lineend > linestart )
        {
            const size_t lastletter = lineend - 1;

            const float linewidth = letters_[lastletter].screenCoord.width_
                                    + letters_[lastletter].screenCoord.x_
                                    - letters_[linestart].screenCoord.x_;

            assert( linewidth > 0.0f );

            const float currentCentreX = letters_[linestart].screenCoord.x_ + linewidth * 0.5f;
            const float indentX = currentCentreX - centreX;
            const float firstLetterXAfterIndent = letters_[linestart].screenCoord.x_ - indentX;
            const float roundedX = round( firstLetterXAfterIndent * screenSize_.width() )
                                   / screenSize_.width();
            const float roundedIndentX = letters_[linestart].screenCoord.x_ - roundedX;

            for( size_t i = linestart; i < lineend; ++i )
            {
                letters_[i].screenCoord.x_ -= roundedIndentX;
            }
        }
    }

    void fontprinter::centre_align_text_block( const float lineheight,
                                               const size_t numletters,
                                               const size_t vertical_line_count )
    {
        if( numletters > 0u )
        {
            const float singlepixelheight = 1.0f / screenSize_.height();
            const float totalheight = lineheight * vertical_line_count;
            const float offsetup = 0.5f * totalheight;

            for( size_t i = 0u; i < numletters; ++i )
            {
                letters_[i].screenCoord.y_ -= offsetup;

                // Move upwards one pixel - purely because many letters have some
                // sort of accent or cap which lowers its baseline. This is purely
                // aesthetic.
                letters_[i].screenCoord.y_ -= singlepixelheight;
            }
        }
    }

    void fontprinter::move_letters_to_be_pixel_perfect( const size_t numletters )
    {
        assert( letters_.size() >= numletters );

        const float singlepixelheight = 1.0f / screenSize_.height();
        const float singlepixelwidth  = 1.0f / screenSize_.width();

        for( size_t i = 0u; i < numletters; ++i )
        {
            // Shift to be on the nearest whole pixel.
            letters_[i].screenCoord.x_ -= fmod( letters_[i].screenCoord.x_, singlepixelwidth );
            letters_[i].screenCoord.y_ -= fmod( letters_[i].screenCoord.y_, singlepixelheight );
        }
    }

    std::pair<position2df, position2df> fontprinter::print_text(
        const utf8text& txt,
        const position2df& screenpos,
        const dimension2df& size,
        HorizontalAlignment halign,
        VerticalAlignment valign,
        const float scale )
    {
        assert( fonttexture_ != texturehandle_t() );

        const size_t nLetters = txt.number_of_characters();

        std::pair<position2df, position2df> result( screenpos, screenpos );

        if( nLetters > 0 )
        {
            const float fieldwidth = size.width();

            const float scaleFactorX = scale / screenSize_.width();
            const float scaleFactorY = scale / screenSize_.height();

            // Find nearest pixel coord to the user's location.
            const float startX = round( screenpos.x() * screenSize_.width() ) / screenSize_.width();
            const float startY = round( screenpos.y() * screenSize_.height() ) / screenSize_.height();

            // Try to keep lineheight so that it steps down in integer pixel
            // values, e.g. 10, not 10.35 pixels, to keep the text clear. Round
            // upwards so as not to shorten the line spacing.
            const float lineheight = ceil( lineheight_ * scale ) / screenSize_.height();

            const float whitespacewidth = ceil( font_->whitespace_size() * scale ) / screenSize_.width();

            const float maxx = fieldwidth > 0.0f ? ( startX + fieldwidth ) : numeric_limits<float>::max();

            // If centre alignment is desired, this is the centre point.
            const float centreX = startX;

            float x = startX;
            float y = startY + lineheight;

            const __m128 scalesYXYX
                = _mm_set_ps( scaleFactorY, scaleFactorX, scaleFactorY, scaleFactorX );

            letters_.resize( nLetters );

            unsigned int iPrintedLetters = 0u;
            unsigned int wordstart = 0u;
            unsigned int linestart = 0u;
            unsigned int vertical_line_count = 1u;
            bool inWord = false;

            // Begin at first non-whitespace character.
            const size_t firstNonWhitespace = find_first_non_whitespace( txt );

            for( size_t i = firstNonWhitespace; i < nLetters; ++i )
            {
                auto codepoint = txt[i];

                auto letter = font_->get_glyph( codepoint );

                // If a space character is zero width the rendering is ruined. This
                // appears to be a bug in the font creator.
                assert( letter.pixel_advanceX() > 0 );

                // Potentially faster if aligned?
                float scaledWHAO[4];
                _mm_storeu_ps( scaledWHAO, _mm_mul_ps( letter.get_pixel_w_h_a_o(), scalesYXYX ) );

                if( is_whitespace( codepoint ) )
                {
                    // Whitespace inserts NO letters.
                    if( inWord )
                    {
                        // End of word detected.
                        inWord = false;

                        if( x > maxx )
                        {
                            // If start of word is not start of line, and word
                            // exceeds the right border, move the whole word down a line.
                            if( letters_[wordstart].screenCoord.x_ > startX )
                            {
                                const float moveX = letters_[wordstart].screenCoord.x_ - startX;

                                // Move each letter in word left.
                                for( size_t iWord = wordstart; iWord < iPrintedLetters; ++iWord )
                                {
                                    letters_[iWord].screenCoord.x_ -= moveX;
                                    letters_[iWord].screenCoord.y_ += lineheight;
                                }

                                if( halign == HorizontalAlignCentre )
                                {
                                    centre_align_line( linestart, wordstart, centreX );
                                }

                                linestart = wordstart;

                                // Move insert point down to next line
                                y += lineheight;

                                // Reset x to end of the word.
                                x = letters_[iPrintedLetters - 1].screenCoord.x_
                                    + letters_[iPrintedLetters - 1].screenCoord.width_;
                            }
                        }
                    }

                    if( is_linebreak( codepoint ) )
                    {
                        // Move insert point down to start of next line
                        x = startX;
                        y += lineheight;
                        ++vertical_line_count;

                        if( halign == HorizontalAlignCentre )
                        {
                            centre_align_line( linestart, iPrintedLetters, centreX );
                        }

                        linestart = iPrintedLetters;

                        // We change loop counter here! Advance to next non
                        // whitespace.
                        i = find_first_non_whitespace( txt, i ) - 1;
                    }
                    else
                    {
                        // Advance insertion point.
                        x += whitespacewidth;//scaledWHAO[2];

                        // If a space character is zero width the rendering is ruined. This
                        // appears to be a bug in the font creator.
                        //assert( scaledWHAO[2] > 0.0f );
                    }
                }
                else
                {
                    // Not whitespace, is letter.
                    if( false == inWord )
                    {
                        // Record start of new word
                        inWord = true;
                        wordstart = iPrintedLetters;
                    }

                    // Add this letter.
                    letters_[iPrintedLetters].texCoord = letter.textureCoords();
                    letters_[iPrintedLetters].screenCoord.x_ = x;
                    letters_[iPrintedLetters].screenCoord.width_ = scaledWHAO[0];  // Scaled Width
                    letters_[iPrintedLetters].screenCoord.height_ = scaledWHAO[1]; // Scaled Height
                    letters_[iPrintedLetters].screenCoord.y_ = y - scaledWHAO[3];  // Scaled Y offset.
                    letters_[iPrintedLetters].cornerColours[0] = fontcolour_;
                    letters_[iPrintedLetters].cornerColours[1] = fontcolour_;
                    letters_[iPrintedLetters].cornerColours[2] = fontcolour_;
                    letters_[iPrintedLetters].cornerColours[3] = fontcolour_;

                    ++iPrintedLetters;

                    // Advance insertion point.
                    x += scaledWHAO[2];
                }
            }

            if( iPrintedLetters > 0 )
            {
                if( halign == HorizontalAlignCentre && ( linestart < iPrintedLetters ) )
                {
                    centre_align_line( linestart, iPrintedLetters, centreX );
                }

                if( valign == VerticalAlignCentre )
                {
                    centre_align_text_block( lineheight, iPrintedLetters, vertical_line_count );
                }

                move_letters_to_be_pixel_perfect( iPrintedLetters );

                renderer_->draw_screen_quads( fonttexture_, &letters_[0], iPrintedLetters );

                const auto letterx = letters_.back().screenCoord.x_;
                const auto lettery = letters_.back().screenCoord.y_;

                const auto finalx = letterx + letters_.back().screenCoord.width_;
                const auto finaly = lettery + letters_.back().screenCoord.height_;

                result.first  = position2df( letterx, lettery );
                result.second = position2df( finalx, finaly );
            }
        }

        return result;
    }
}
