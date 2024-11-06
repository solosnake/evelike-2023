#include "solosnake/font.hpp"
#include "solosnake/itextureloader.hpp"

using namespace std;

namespace solosnake
{
    font::font(
        const filepath& fontfile,
        itextureloader& texloader )
        : fontletters( fontfile )
        , font_texture_( texturehandle_t() )
    {
        font_texture_ = texloader.get_texture( fontletters::texture_filename() );
    }

    font::font(
        const fontletters& letters,
        itextureloader& texloader )
        : fontletters( letters )
        , font_texture_( texloader.get_texture( letters.texture_filename() ) )
    {
    }

    font& font::operator=( const font& rhs )
    {
        if( this != &rhs )
        {
            fontletters::operator=( rhs );
            font_texture_ = rhs.font_texture_;
        }

        return *this;
    }

    font& font::operator=( font && rhs )
    {
        if( this != &rhs )
        {
            fontletters::operator=( rhs );
            font_texture_ = std::move( rhs.font_texture_ );
        }

        return *this;
    }
}
