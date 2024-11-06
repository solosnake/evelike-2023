#include "solosnake/fontcache.hpp"
#include "solosnake/font.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/throw.hpp"
#include <algorithm>
#include <functional>
#include <string>

using namespace std;

namespace solosnake
{
    namespace
    {
        //! Proxy class implementing the itextureloader interface via
        //! a functor.
        class itextureloaderproxy : public itextureloader
        {
        public:

            typedef std::function<texturehandle_t( const std::string& )> tex_getter_fn_t;

            itextureloaderproxy( tex_getter_fn_t f ) : tex_getter_( f )
            {
            }

            texturehandle_t get_texture( const std::string& name ) override
            {
                return tex_getter_( name );
            }

        private:

            tex_getter_fn_t tex_getter_;
        };
    }

    fontcache::fontcache( const std::shared_ptr<ifilefinder>& fontspaths )
      : fontspaths_( fontspaths )
    {
    }

    fontcache::~fontcache()
    {
    }

    shared_ptr<font> fontcache::get_font( const std::string& fontfilename )
    {
        auto fontUrl = fontspaths_->get_file( fontfilename );

        auto compareUrls = [&]( const loaded_font_t& f )
        { return ( f.first == fontUrl ); };

        auto get_tex = [&]( const std::string & name )
        { return this->get_font_texture( name ); };

        auto i = find_if( loadedFonts_.cbegin(), loadedFonts_.cend(), compareUrls );

        if( i != loadedFonts_.cend() )
        {
            return i->second;
        }

        // Construct and use cast, otherwise make_shared gets confused.
        itextureloaderproxy proxyloader( get_tex );

        shared_ptr<font> newFont = make_shared<font>( fontUrl, static_cast<itextureloader&>( proxyloader ) );

        loadedFonts_.push_back( loaded_font_t( fontUrl, newFont ) );

        return newFont;
    }
}
