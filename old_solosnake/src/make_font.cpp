#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "solosnake/bestfit.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/image.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/make_font.hpp"
#include "solosnake/position.hpp"
#include "solosnake/rect.hpp"
#include "solosnake/throw.hpp"

using namespace std;

#define SOLOSNAKE_INVALID_GLYPH             (0)
#define SOLOSNAKE_FT_INVALID_GLYPH_INDEX    (0)

namespace solosnake
{
    // This is 'internal detail' code of the solosnake namespace.
    namespace
    {
        //! Represents a letter character which will be written to a complete
        //! texture font. This knows the codepoint and the location of the
        //! rectangle on the final texture which represents the character.
        //! All methods take into account whether the image of the character the
        //! glyph owns will be rotated when placed into the final composite
        //! texture image.
        class glyph
        {
        public:

            //! When an image is rotated, it is always considered rotated
            //! by 90 degrees clockwise.
            glyph(
                uint32_t codepoint,
                int advanceX,
                int yOffset,
                std::shared_ptr<image> );

            glyph(
                const glyph&,
                const std::vector<uint32_t>& additionalCodepoints );

            unsigned int width() const;

            unsigned int height() const;

            int offsetY() const;

            int advanceX() const;

            bool is_rotated() const;

            void set_is_rotated( bool );

            fontglyph letter( const dimension2d<unsigned int>& bitmapSize ) const;

            const unsigned char* pixel( unsigned int x, unsigned int y ) const;

            void set_location_on_image( const position2d<unsigned int>& );

            const position2d<unsigned int>& location_on_image() const;

            const vector<uint32_t>& codepoints() const;

            void save_image( const char* ) const;

            bool has_same_fontglyph( const glyph& ) const;

        private:

            std::shared_ptr<image>      image_;
            position2d<unsigned int>    positionOnFinalImage_;
            int                         advanceX_;
            int                         yOffset_;
            vector<uint32_t>            codepoints_;
            bool                        is_rotated_;
        };

        //-------------------------------------------------------------------------

        glyph::glyph( uint32_t codepoint, int advanceX, int yOffset, std::shared_ptr<image> imgp )
            : image_( imgp )
            , positionOnFinalImage_( 0, 0 )
            , advanceX_( advanceX )
            , yOffset_( yOffset )
            , codepoints_( 1, codepoint )
            , is_rotated_( false )
        {
            assert( codepoints_.at( 0 ) == codepoint );
        }

        glyph::glyph( const glyph& other, const std::vector<uint32_t>& additionalCodepoints )
            : image_( other.image_ )
            , positionOnFinalImage_( other.positionOnFinalImage_ )
            , advanceX_( other.advanceX_ )
            , yOffset_( other.yOffset_ )
            , codepoints_( other.codepoints_ )
            , is_rotated_( other.is_rotated_ )
        {
            codepoints_.reserve( other.codepoints_.size() + additionalCodepoints.size() );
            copy( additionalCodepoints.cbegin(),
                  additionalCodepoints.cend(),
                  back_inserter( codepoints_ ) );
        }

        const position2d<unsigned int>& glyph::location_on_image() const
        {
            return positionOnFinalImage_;
        }

        const vector<uint32_t>& glyph::codepoints() const
        {
            return codepoints_;
        }

        fontglyph glyph::letter( const dimension2d<unsigned int>& bitmapSize ) const
        {
            texquad texCoords;

            texCoords.x0 = static_cast<float>( positionOnFinalImage_.x() ) / bitmapSize.width();
            texCoords.y0 = static_cast<float>( positionOnFinalImage_.y() ) / bitmapSize.height();

            texCoords.x1 = static_cast<float>( positionOnFinalImage_.x() + width() ) / bitmapSize.width();
            texCoords.y1 = static_cast<float>( positionOnFinalImage_.y() + height() ) / bitmapSize.height();

            if( is_rotated() )
            {
                swap( texCoords.y0, texCoords.y1 );
            }

            // We need to use the image width and height here. The rotation
            // is now stored in the texture coordinates, and the fontglyph is
            // only interested in the width and height of the letter therein.
            return fontglyph( texCoords, image_->width(), image_->height(), advanceX_, yOffset_ );
        }

        bool glyph::has_same_fontglyph( const glyph& rhs ) const
        {
            return advanceX_   == rhs.advanceX_   &&
                   yOffset_    == rhs.yOffset_    &&
                   is_rotated_ == rhs.is_rotated_ &&
                   *image_     == *rhs.image_;
        }

        bool glyph::is_rotated() const
        {
            return is_rotated_;
        }

        void glyph::set_is_rotated( bool r )
        {
            is_rotated_ = r;
        }

        void glyph::set_location_on_image( const position2d<unsigned int>& xy )
        {
            positionOnFinalImage_ = xy;
        }

        void glyph::save_image( const char* name ) const
        {
            image_->save( name );
        }

        unsigned int glyph::width() const
        {
            return is_rotated() ? image_->height() : image_->width();
        }

        unsigned int glyph::height() const
        {
            return is_rotated() ? image_->width() : image_->height();
        }

        int glyph::offsetY() const
        {
            return yOffset_;
        }

        int glyph::advanceX() const
        {
            return advanceX_;
        }

        const unsigned char* glyph::pixel( unsigned int x, unsigned int y ) const
        {
            return image::pixel( x, y, *image_, image_->format() );
        }

        typedef shared_ptr<glyph> glyph_ptr;

        //-------------------------------------------------------------------------

        //! Manages the FreeType library lifetime.
        class FreeTypeLib
        {
            mutable FT_Library library_;

        public:

            FreeTypeLib()
            {
                if( 0 != FT_Init_FreeType( &library_ ) )
                {
                    throw runtime_error( "Failed to initialise FreeType." );
                }
            }

            ~FreeTypeLib()
            {
                FT_Done_FreeType( library_ );
            }

            operator FT_Library& () const
            {
                return library_;
            }
        };

        //-------------------------------------------------------------------------

        class FreeTypeFace
        {
            unique_ptr<FreeTypeLib> library_;
            mutable FT_Face face_;

            static FT_Face load_face( FT_Library& lib, const char* file )
            {
                FT_Face face;

                switch( FT_New_Face( lib, file, 0, &face ) )
                {
                    case 0:
                        break;

                    case FT_Err_Unknown_File_Format:
                        // the font file could be opened and read, but it appears
                        // that its font format is unsupported
                        throw runtime_error( "Unsupported font type." );
                        break;

                    default:
                        // another error code means that the font file could not
                        // be opened or read, or simply that it is broken...
                        throw runtime_error( "Unable to open / read font file." );
                        break;
                }

                return face;
            }

        public:

            explicit FreeTypeFace( const char* fontname ) : library_( new FreeTypeLib() )
            {
                face_ = load_face( *library_, fontname );
            }

            ~FreeTypeFace()
            {
                FT_Done_Face( face_ );
            }

            operator FT_Face& () const
            {
                return face_;
            }

        private:
            FreeTypeFace( const FreeTypeFace& );
            FreeTypeFace& operator=( const FreeTypeFace& );
        };

        //-------------------------------------------------------------------------

        //! Returns an array of rectangles with a 1:1 ordering and size of the
        //! glyphs.
        vector<bestfit::rectangle>
        make_vector_of_padded_rects_from_glyphs( const vector<glyph_ptr>& glyphs,
                                                 const unsigned int pixel_padding )
        {
            vector<bestfit::rectangle> result;
            result.reserve( glyphs.size() );

            transform(
                glyphs.cbegin(),
                glyphs.cend(),
                back_inserter( result ),
                [&]( const glyph_ptr & g )
            {
                return bestfit::rectangle( pixel_padding + g->width(),
                                           pixel_padding + g->height() );
            }
            );

            return result;
        }

        void write_glyph_to_image( image& pic, glyph_ptr pgly )
        {
            assert( pic.format() == image::format_bgra );

            const glyph& gly = *pgly;

            auto glyphPosition = gly.location_on_image();

            for( unsigned int i = 0u; i < gly.width(); ++i )
            {
                for( unsigned int j = 0u; j < gly.height(); ++j )
                {
                    const unsigned char* srcBGRA = gly.pixel( i, j );

                    unsigned char* dstBGRA =
                        image::pixel( glyphPosition.x() + i,
                                      glyphPosition.y() + j,
                                      pic,
                                      image::format_bgra );

                    dstBGRA[0] = srcBGRA[0];
                    dstBGRA[1] = srcBGRA[1];
                    dstBGRA[2] = srcBGRA[2];
                    dstBGRA[3] = srcBGRA[3];
                }
            }
        }

        //! Attempts to calculate a decent 2D layout of the glyphs that will produce
        //! an image of least rectangular area. Sets each glyphs bitmap location./
        bestfit::rectangle
        calculate_small_layout( const vector<glyph_ptr>& glyphs,
                                const unsigned int pixel_padding )
        {
            using namespace bestfit;

            const auto bestlayout = best_fit_rectangle_containing(
                                        make_vector_of_padded_rects_from_glyphs( glyphs, pixel_padding ) );

            assert( bestlayout.size() == glyphs.size() );

            for( size_t i = 0u; i < bestlayout.size(); ++i )
            {
                assert( false == bestlayout[i].is_rotated() ); // Not handled right now.
                glyphs[bestlayout[i].index()]->set_is_rotated( bestlayout[i].is_rotated() );
                glyphs[bestlayout[i].index()]->set_location_on_image( bestlayout[i].position() );
            }

            return bounding_rectangle( bestlayout );
        }

        inline void set_pixel_rgba( std::uint8_t value, int x, int y, image& img )
        {
            *reinterpret_cast<bgra*>( image::pixel( x, y, img, image::format_bgra ) ) = bgra( value );
        }

        std::shared_ptr<image> make_image_from_ftbitmap( const FT_Bitmap& ftbmp )
        {
            auto img = make_shared<image>( ftbmp.width, ftbmp.rows, image::format_bgra );

            const int w = ftbmp.width;
            const int h = ftbmp.rows;

            for( int y = 0; y < h; ++y )
            {
                for( int x = 0; x < w; ++x )
                {
                    set_pixel_rgba( ftbmp.buffer[x + y * w], x, y, *img );
                }
            }

            return img;
        }

        glyph_ptr make_glyp_ptr( FT_Face& face, uint32_t codepoint )
        {
            return make_shared<glyph>( codepoint,
                                       face->glyph->advance.x / 64,
                                       face->glyph->bitmap_top, // How many pixels UP
                                       make_image_from_ftbitmap( face->glyph->bitmap ) );
        }

        //! Can return an empty pointer if the codepoint is not found in face.
        glyph_ptr try_create_glyph( FT_Face& face, uint32_t codepoint )
        {
            glyph_ptr result;

            if( codepoint != SOLOSNAKE_INVALID_GLYPH
                    && 0 == FT_Load_Char( face, codepoint, FT_LOAD_RENDER ) )
            {
                if( face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0 )
                {
                    result = make_glyp_ptr( face, codepoint );
                }
            }

            return result;
        }

        //! Always returns the FT invalid codepoint glyph.
        glyph_ptr make_invalid_character_glyph( FT_Face& face )
        {
            glyph_ptr result;

            auto glyphLoaded = FT_Load_Char( face, SOLOSNAKE_FT_INVALID_GLYPH_INDEX, FT_LOAD_RENDER );

            if( 0 == glyphLoaded )
            {
                if( face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0 )
                {
                    result = make_glyp_ptr( face, SOLOSNAKE_INVALID_GLYPH );
                }
                else
                {
                    // Loaded but no bitmap?
                    ss_wrn( "Face bitmap width or height was zero" );

                    const auto width  = face->glyph->metrics.width  / 64U;
                    const auto height = face->glyph->metrics.height / 64U;

                    if( width > 0 && height > 0 )
                    {
                        auto whitesquare = make_shared<image>( width, height, image::format_bgra );

                        whitesquare->clear_to_bgra( 0xFF, 0xFF, 0xFF, 0xFF );

                        result = make_shared<glyph>( SOLOSNAKE_INVALID_GLYPH, width, height, whitesquare );
                    }
                }
            }

            if( nullptr == result.get() )
            {
                ss_wrn( "Font does not support 'invalid' glyph - replacing with white square." );

                unsigned int mw = std::max<unsigned int>( 1U, std::abs( face->max_advance_width )  / 64U );
                unsigned int mh = std::max<unsigned int>( 1U, std::abs( face->max_advance_height ) / 64U );

                auto whitesquare = make_shared<image>( mw, mh, image::format_bgra );

                whitesquare->clear_to_bgra( 0xFF, 0xFF, 0xFF, 0xFF );

                result = make_shared<glyph>( SOLOSNAKE_INVALID_GLYPH, mw, mh, whitesquare );
            }

            return result;
        }

        //! Creates a glyph (an image) of each character in the array using the
        //! given face, and returns the array.
        //! Always inserts the "invalid character" glyph at index zero.
        vector<glyph_ptr>
        print_characters_to_glyphs( const FreeTypeFace* const face,
                                    const vector<uint32_t>& characters,
                                    const bool verbose )
        {
            vector<glyph_ptr> glyphs;
            glyphs.reserve( 1 + characters.size() );

            glyphs.push_back( make_invalid_character_glyph( *face ) );

            for( size_t i = 0; i < characters.size(); ++i )
            {
                auto g = try_create_glyph( *face, characters[i] );

                if( g )
                {
                    glyphs.push_back( g );
                }
                else if( verbose )
                {
                    ss_log( "No character found for codepoint ", characters[i] );
                }
            }

            return glyphs;
        }

        //! Copies each glyph onto the new image at the glyph's bitmap location.
        std::shared_ptr<image>
        make_font_texture( const bestfit::rectangle& bbox,
                           const vector<glyph_ptr>& glyphs )
        {
            std::shared_ptr<image> fonttex = make_shared<image>( bbox.width(), bbox.height(), image::format_bgra );

            fonttex->clear_to_bgra( 0, 0, 0, 0 );

            for( size_t i = 0; i < glyphs.size(); ++i )
            {
                write_glyph_to_image( *fonttex, glyphs[i] );
            }

            return fonttex;
        }

        std::shared_ptr<fontletters>
        make_fontletters( const string& imageName,
                          const dimension2d<unsigned int>& imageSize,
                          const vector<glyph_ptr>& glyphs,
                          unsigned int lineSpacing,
                          unsigned int whitespaceSize )
        {
            std::map<std::uint32_t, fontglyph> codepoints;

            for( size_t i = 0; i < glyphs.size(); ++i )
            {
                auto letter = glyphs[i]->letter( imageSize );
                for( size_t j = 0; j < glyphs[i]->codepoints().size(); ++j )
                {
                    // Check each codepoint occurs only once.
                    assert( codepoints.count( glyphs[i]->codepoints()[j] ) == 0 );
                    codepoints[ glyphs[i]->codepoints()[j] ] = letter;
                }
            }

            // Find the unknown (codepoint zero) and remove it.
            assert( codepoints.count( 0 ) == 1 );
            fontglyph unknown = codepoints[0];
            codepoints.erase( 0 );

            auto letters = make_shared<fontletters>( imageName, codepoints, unknown, lineSpacing, whitespaceSize );

            return letters;
        }

        //! Finds the line spacing based on the tallest glyph.
        unsigned int face_line_spacing( const vector<glyph_ptr>& glyphs )
        {
            assert( false == glyphs.empty() );

            // This is supposed to return the value we want:
            // return face->height / 64.0f;
            // However it does not! So we return the height of the letter
            //  with the most above the line.
            int highestOverLine = 0;

            for_each( glyphs.begin(),
                      glyphs.end(),
                      [&]( const glyph_ptr & p )
            { highestOverLine = max( highestOverLine, p->offsetY() ); } );

            assert( highestOverLine > 0 );

            return ( highestOverLine > 0 ) ? static_cast<unsigned int>( highestOverLine ) : 0u;
        }

        //! If there is a whitespace (ASCII 32) codepoint, its width is used, else
        //! the width is based on the most frequently occurring letter codepoint.
        unsigned int face_whitespace_width( const vector<glyph_ptr>& glyphs )
        {
            const unsigned int asciiSpace = 32u;

            unsigned int spacewidth = 0;

            // We will take the most common letter width as the space width.
            map<unsigned int, size_t> freqOfAdvanceXs;
            size_t highesFreq = 0;
            unsigned int mostcommonAdvanceX = 0;

            for_each( glyphs.cbegin(),
                      glyphs.cend(),
                      [&]( const glyph_ptr & p )
            {
                const auto ax = p->advanceX();

                if( p->codepoints().size() == 1 && p->codepoints().at( 0 ) == asciiSpace )
                {
                    spacewidth = ax;
                }

                freqOfAdvanceXs[ ax ] += p->codepoints().size();

                if( freqOfAdvanceXs[ ax ] >= highesFreq )
                {
                    highesFreq = freqOfAdvanceXs[ ax ];
                    mostcommonAdvanceX = ax;
                }
            } );

            // Return most common if no space width found.
            return spacewidth > 0 ? spacewidth : mostcommonAdvanceX;
        }

        vector<glyph_ptr> merge_identical_glyphs( const vector<glyph_ptr>& glyphs )
        {
            const size_t N = glyphs.size();

            if( N < 1u )
            {
                return glyphs;
            }

            vector<glyph_ptr> mergedGlyphs;
            mergedGlyphs.reserve( glyphs.size() );

            // Record which glyphs have been 'removed'.
            vector<bool> removed( glyphs.size(), false );
            vector<uint32_t> codepointsWithSameData;
            codepointsWithSameData.reserve( glyphs.size() );

            for( size_t i = 0u; i < N; ++i )
            {
                codepointsWithSameData.clear();
                if( false == removed[i] )
                {
                    for( size_t j = i + 1u; j < N; ++j )
                    {
                        if( false == removed[j] )
                        {
                            if( glyphs[j]->has_same_fontglyph( *glyphs[i] ) )
                            {
                                removed[j] = true;
                                copy( glyphs[j]->codepoints().cbegin(),
                                      glyphs[j]->codepoints().cend(),
                                      back_inserter( codepointsWithSameData ) );
                            }
                        }
                    }

                    mergedGlyphs.push_back( make_shared<glyph>( *glyphs[i], codepointsWithSameData ) );
                }
            }

            return mergedGlyphs;
        }
    }

    //-------------------------------------------------------------------------

    //! @note See this excellent post:
    // http://lists.gnu.org/archive/html/freetype-devel/2004-02/msg00018.html
    pair<shared_ptr<image>, shared_ptr<fontletters>>
                                                  make_font( const string& fontfile,
                                                             const string& texfilename,
                                                             const vector<uint32_t>& characters,
                                                             const unsigned int font_pixels_size,
                                                             const unsigned int pixel_padding,
                                                             const bool verbose )
    {
        using namespace bestfit;

        unique_ptr<FreeTypeFace> face( new FreeTypeFace( fontfile.c_str() ) );

        if( 0 == FT_Set_Pixel_Sizes( *face, 0, font_pixels_size ) )
        {
            if( verbose )
            {
                ss_log( "Printing glyphs..." );
            }

            vector<glyph_ptr> glyphs = print_characters_to_glyphs( face.get(), characters, verbose );

            glyphs = merge_identical_glyphs( glyphs );

            if( verbose )
            {
                ss_log( "Calculating layout..." );
            }

            bestfit::rectangle bbox = calculate_small_layout( glyphs, pixel_padding );

            if( verbose )
            {
                ss_log( "Building texture..." );
            }

            std::shared_ptr<image> resultImg = make_font_texture( bbox, glyphs );

            if( verbose )
            {
                ss_log( "Building fontletters..." );
            }

            auto resultFontletters =
                make_fontletters( texfilename,
                                  dimension2d<unsigned int>( bbox.width(), bbox.height() ),
                                  glyphs,
                                  face_line_spacing( glyphs ),
                                  face_whitespace_width( glyphs ) );

            return make_pair( resultImg, resultFontletters );
        }
        else
        {
            throw runtime_error( "Unable to set requested character size." );
        }
    }
}
