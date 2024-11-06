#include <fstream>
#include <filesystem>
#include <utility>
#include "solosnake/filepath.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/html.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/utf8text.hpp"

using namespace std;

#define SOLOSNAKE_CODEPOINT_XML_ELEMENT "codepoint"

namespace solosnake
{
    namespace
    {
        //! Returns a zero'ed default letter.
        fontglyph zero_letter()
        {
            texquad coords;
            coords.x0 = 0.0f;
            coords.y0 = 0.0f;
            coords.x1 = 0.0f;
            coords.y1 = 0.0f;

            return fontglyph( coords, 0, 0, 0, 0 );
        }

        class fontxmlreader : public ixmlelementreader
        {
            typedef pair<uint32_t, fontglyph> codepoint_t;

            bool is_codepoint( const TiXmlElement& xml ) const
            {
                return 0 == xml.ValueStr().compare( SOLOSNAKE_CODEPOINT_XML_ELEMENT );
            }

            //! Returns zero if XML element contains a non UTF-8 element.
            uint32_t read_utf8_codepoint( const TiXmlElement& xml ) const
            {
                string utf8string;
                solosnake::read_attribute( xml, "utf-8", utf8string, false );

                solosnake::utf8text txt( utf8string );

                if( txt.number_of_characters() > 1 )
                {
                    ss_err( "Invalid 'utf-8' font element ", utf8string, " (", xml.GetText(), ")" );
                }

                return txt.number_of_characters() == 0 ? 0 : txt[0];
            }

            codepoint_t read_codepoint( const TiXmlElement& xml ) const
            {
                uint32_t cp = read_utf8_codepoint( xml );

                texquad texCoords;
                solosnake::read_attribute( xml, "x0", texCoords.x0, false );
                solosnake::read_attribute( xml, "y0", texCoords.y0, false );
                solosnake::read_attribute( xml, "x1", texCoords.x1, false );
                solosnake::read_attribute( xml, "y1", texCoords.y1, false );

                unsigned int pixelwidth;
                unsigned int pixelheight;
                unsigned int advanceX;
                solosnake::read_attribute( xml, "pixelwidth", pixelwidth, false );
                solosnake::read_attribute( xml, "pixelheight", pixelheight, false );
                solosnake::read_attribute( xml, "advanceX", advanceX, false );

                int yOffset = 0;
                solosnake::read_attribute( xml, "offsetY", yOffset, true );

                return make_pair( cp, fontglyph( texCoords, pixelwidth, pixelheight, advanceX, yOffset ) );
            }

        public:

            fontxmlreader()
                : ixmlelementreader( "codepoints" )
                , unknown_letter_( zero_letter() )
                , line_spacing_( 0 )
                , whitespace_size_( 0 )
            {
            }

            bool read_group_element( const TiXmlElement& xml ) override
            {
                return solosnake::read_attribute( xml, "texture", textureName_, false ) &&
                       solosnake::read_attribute( xml, "lineheight", line_spacing_, false ) &&
                       solosnake::read_attribute( xml, "white_space_size", whitespace_size_, false );
            }

            bool read_from_element( const TiXmlElement& xml ) override
            {
                bool goodRead = false;

                if( is_codepoint( xml ) )
                {
                    // We expect multiple codepoints:
                    auto cp = read_codepoint( xml );

                    if( codepoints_.count( cp.first ) == 0 )
                    {
                        if( cp.first > 0 )
                        {
                            codepoints_.insert( cp );
                        }
                        else
                        {
                            // Use this as unknown. We may in theory hit this
                            // more than once.
                            unknown_letter_ = cp.second;
                        }

                        goodRead = true;
                    }
                    else
                    {
                        ss_err( "Codepoint was declared more than once : ", cp.first );
                    }
                }
                else
                {
                    ss_err( "Expected XML element '" SOLOSNAKE_CODEPOINT_XML_ELEMENT "', found '",
                            xml.ValueStr(),
                            "'" );
                }

                return goodRead;
            }

            string                      textureName_;
            map<uint32_t, fontglyph>    codepoints_;
            fontglyph                   unknown_letter_;
            unsigned int                line_spacing_;
            unsigned int                whitespace_size_;
        };

        void write_codepoint( ostream& os, uint32_t cp, const fontglyph& gly )
        {
            os << "<codepoint utf-8=\"" << htmlescape( utf8text( cp ) ) << "\" x0=\""
               << gly.textureCoords().x0 << "\" y0=\"" << gly.textureCoords().y0 << "\" x1=\""
               << gly.textureCoords().x1 << "\" y1=\"" << gly.textureCoords().y1 << "\" pixelwidth=\""
               << gly.letter_pixel_width() << "\" pixelheight=\"" << gly.letter_pixel_height()
               << "\" advanceX=\"" << gly.pixel_advanceX() << "\"";

            // Offset is optional when zero.
            if( gly.pixel_offsetY() != 0 )
            {
                os << " offsetY=\"" << gly.pixel_offsetY() << "\"";
            }

            os << " />\n";
        }
    }

    //-------------------------------------------------------------------------

    fontletters::fontletters( const filepath& fontfile )
        : unknown_letter_( zero_letter() )
        , line_spacing_( 0 )
        , whitespace_size_( 0 )
    {
        assert( std::filesystem::exists( fontfile ) );

        fontxmlreader xmlfile;
        if( ixmlelementreader::read( fontfile.string().c_str(), xmlfile ) )
        {
            textureFilename_ = std::move( xmlfile.textureName_ );
            unknown_letter_  = std::move( xmlfile.unknown_letter_ );
            codepoints_      = std::move( xmlfile.codepoints_ );
            line_spacing_    = xmlfile.line_spacing_;
            whitespace_size_ = xmlfile.whitespace_size_;
        }
        else
        {
            ss_err( "Problem loading xml fontletters file : ", fontfile.string() );
            ss_throw( "Unable to load fontletters file." );
        }
    }

    fontletters::fontletters( const std::string& textureFilename,
                              const std::map<std::uint32_t, fontglyph>& codepoints,
                              const fontglyph& unknownLetter,
                              const unsigned int lineSpacing,
                              const unsigned int whitespaceSize )
        : textureFilename_( textureFilename )
        , codepoints_( codepoints )
        , unknown_letter_( unknownLetter )
        , line_spacing_( lineSpacing )
        , whitespace_size_( whitespaceSize )
    {
        if( textureFilename_.empty() )
        {
            ss_throw( "fontletters texture name was empty." );
        }
    }

    fontletters::fontletters( fontletters&& other )
    {
        *this = move( other );
    }

    fontletters::fontletters( const fontletters& rhs )
        : textureFilename_( rhs.textureFilename_ )
        , codepoints_( rhs.codepoints_ )
        , unknown_letter_( rhs.unknown_letter_ )
        , line_spacing_( rhs.line_spacing_ )
    {
    }

    fontletters& fontletters::operator=( fontletters && other )
    {
        if( this != &other )
        {
            textureFilename_ = move( other.textureFilename_ );
            codepoints_      = move( other.codepoints_ );
            unknown_letter_  = move( other.unknown_letter_ );
            line_spacing_    = move( other.line_spacing_ );
        }

        return *this;
    }

    fontletters& fontletters::operator=( const fontletters& other )
    {
        if( this != &other )
        {
            textureFilename_ = other.textureFilename_;
            codepoints_      = other.codepoints_;
            unknown_letter_  = other.unknown_letter_;
            line_spacing_    = other.line_spacing_;
        }

        return *this;
    }

    //! Virtual dtor to allow safe inheritance.
    fontletters::~fontletters()
    {
    }

    //! Returns the indexed letter, or the default codepoint.
    const fontglyph& fontletters::get_glyph( uint32_t letter ) const
    {
        auto it = codepoints_.find( letter );
        return codepoints_.end() == it ? unknown_letter_ : it->second;
    }

    bool fontletters::save( std::ostream& os ) const
    {
        os <<
           "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
           "<codepoints version=\"1.0\" "
           "lineheight=\""
           << line_spacing_
           << "\" white_space_size=\""
           << whitespace_size_
           << "\" texture=\""
           << textureFilename_
           << "\">\n";

        // Write unknown as codepoint zero.
        write_codepoint( os, 0, unknown_letter_ );

        for_each(
            codepoints_.cbegin(),
            codepoints_.cend(),
            [&]( const map<uint32_t, fontglyph>::value_type & val )
        {
            write_codepoint( os, val.first, val.second );
        } );

        os << "</codepoints>";

        return os.good();
    }

    bool fontletters::save( const char* filename ) const
    {
        if( filename )
        {
            std::ofstream ofile( filename );
            return ofile.is_open() ? save( ofile ) : false;
        }

        return false;
    }

    bool fontletters::operator==( const fontletters& rhs ) const
    {
        return ( textureFilename_ == rhs.textureFilename_ ) && ( codepoints_ == rhs.codepoints_ )
               && ( unknown_letter_ == rhs.unknown_letter_ ) && ( line_spacing_ == rhs.line_spacing_ );
    }

    bool fontletters::operator!=( const fontletters& rhs ) const
    {
        return false == ( ( *this ) == rhs );
    }
}
