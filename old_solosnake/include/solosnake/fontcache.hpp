#ifndef solosnake_font_cache_hpp
#define solosnake_font_cache_hpp

#include <vector>
#include <utility>
#include <memory>
#include "solosnake/filepath.hpp"
#include "texturehandle.hpp"

namespace solosnake
{
    class font;
    class ifilefinder;

    //! Ensures fonts are only loaded once. This class is designed to be
    //! inherited from and is an interface class - it requires a texture loader
    //! interface to work correctly and thus interacts strongly with a renderer.
    //! It thus cannot be created or used before a renderer is present.
    class fontcache
    {
    public:

        virtual ~fontcache();

        std::shared_ptr<font> get_font( const std::string& fontfile );

    protected:

        explicit fontcache( const std::shared_ptr<ifilefinder>& );

        virtual texturehandle_t get_font_texture( const std::string& ) = 0;

    private:

        typedef std::pair<filepath, std::shared_ptr<font>> loaded_font_t;

        std::shared_ptr<ifilefinder>        fontspaths_;
        mutable std::vector<loaded_font_t>  loadedFonts_;
    };
}

#endif
