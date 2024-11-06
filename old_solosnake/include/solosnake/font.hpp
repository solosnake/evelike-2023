#ifndef solosnake_font_hpp
#define solosnake_font_hpp

#include <cstdint>
#include <map>
#include <memory>
#include "solosnake/filepath.hpp"
#include "solosnake/fontletters.hpp"
#include "solosnake/texturehandle.hpp"

namespace solosnake
{
    class itextureloader;

    //! A font is used in actual rendering. It knows the texture coordinates
    //! and has a loaded texture handle. It should only be used while its
    //! renderer is valid.
    class font : public fontletters
    {
    public:

        font( const filepath& fontfile, itextureloader& );

        font( const fontletters&, itextureloader& );

        font( const font& f );

        font( font&& );

        texturehandle_t texhandle() const;

        font& operator=( const font& );

        font& operator=( font && );

    private:

        texturehandle_t font_texture_;
    };

    //-------------------------------------------------------------------------

    inline font::font( const font& f ) : fontletters( f ), font_texture_( f.font_texture_ )
    {
    }

    inline font::font( font&& rhs ) : fontletters( rhs ), font_texture_( rhs.font_texture_ )
    {
    }

    inline texturehandle_t font::texhandle() const
    {
        return font_texture_;
    }
}

#endif
