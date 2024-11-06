#ifndef solosnake_fontprinter_hpp
#define solosnake_fontprinter_hpp

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include "solosnake/alignment.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/texturehandle.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/position.hpp"
#include "solosnake/bgra.hpp"

namespace solosnake
{
    class utf8text;
    class font;
    class window;

    //! Prints UTF-8 text in a single font of a single colour to a
    //! rectangular area on-screen.
    //! Whitespace letter's will not be written, although their
    //! values will be used to format the text.
    //! Default text colour is white and fully opaque.
    class fontprinter
    {
    public:

        //! Create fontprinter with default colour of white.
        fontprinter( const dimension2d<unsigned int>& screenSize,
                     const std::shared_ptr<deferred_renderer>&,
                     const std::shared_ptr<font>& );

        void set_font( const std::shared_ptr<font>& );

        void set_screen_size( const dimension2d<unsigned int>& );

        void set_text_colour( const bgra& );

        bgra text_colour() const;

        //! For formatting we use the rule:
        //! "If word ends AND word-length exceeds right border AND word[0].x >
        //! left border THEN move word down a line."
        //! \param txt          UTF-8 text to be printer. Leading whitespace will be skipped.
        //! \param screenpos    The location of the text onscreen.
        //!                     In AlignLeft mode, the text will be written beginning at
        //!                     the pixel closest to the x and y position.
        //!                     In AlignCentre mode, the screenpos will the point
        //!                     the text is centred about.
        //! \param size         The size width and height defines the area the text will be
        //!                     restricted to and optionally aligned.
        //!                     Lines that exceed the width will be wrapped
        //!                     to a new line.
        //!                     If the size's width is zero, line wrapping is disabled.
        //! \param halign       The horizontal alignment mode, this affects the interpretation
        //!                     of the screenpos param. When centre then each line will be centred
        //!                     horizontally about the x position of screenpos. When left
        //!                     aligned, then the x is the location the text begins at.
        //! \param valign       The vertical alignment mode, this affects the interpretation
        //!                     of the screenpos param. When centre than the entire text block
        //!                     is shifted to that its middle line is at screenpos.y.
        //!                     When top, the top of the text is aligned to the screenpos.y.
        //! \param scale        Scale applied to the letters. Default is 1.0.
        //!
        //! \returns            Returns the top/left (first) and bottom/right (second) of the 
        //!                     last letter printed onscreen. To print another line or letter 
        //!                     immediately following this letter, use the first value as the 
        //!                     screenpos argument to the next call to print_text.
        //!                     To begin a new line, re-use the screenpos x from this call, but 
        //!                     use the y value from the second returned pair.
        //!                     If no letters were printed, or if there is a formatting error,
        //!                     then the first and second pair will be the same values.
        std::pair<position2df, position2df> print_text(
            const utf8text& txt,
            const position2df& screenpos,
            const dimension2df& size,
            HorizontalAlignment halign,
            VerticalAlignment valign,
            const float scale = 1.0f );

    private:

        static inline float round( float );

        inline dimension2df pixelcoords( float x, float y ) const;

        void centre_align_line(
            const size_t linestart,
            const size_t lineend,
            const float centreX );

        void centre_align_text_block(
            const float lineheight,
            const size_t numletters,
            const size_t vertical_line_count );

        void move_letters_to_be_pixel_perfect( const size_t numletters );

        std::shared_ptr<deferred_renderer>          renderer_;
        std::shared_ptr<font>                       font_;
        std::vector<deferred_renderer::screen_quad> letters_;
        dimension2d<unsigned int>                   screenSize_;
        texturehandle_t                             fonttexture_;
        float                                       lineheight_;
        bgra                                        fontcolour_;
    };

    //-------------------------------------------------------------------------

    inline void fontprinter::set_text_colour( const bgra& c )
    {
        fontcolour_ = c;
    }

    inline bgra fontprinter::text_colour() const
    {
        return fontcolour_;
    }

    inline float fontprinter::round( float number )
    {
        return number < 0.0f ? ceil( number - 0.5f ) : floor( number + 0.5f );
    }

    inline dimension2df fontprinter::pixelcoords( float x, float y ) const
    {
        return dimension2df( round( x * screenSize_.width() ) / screenSize_.width(),
                             round( y * screenSize_.height() ) / screenSize_.height() );
    }
}

#endif
