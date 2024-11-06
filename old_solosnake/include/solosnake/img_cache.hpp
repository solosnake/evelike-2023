#ifndef solosnake_img_cache_hpp
#define solosnake_img_cache_hpp

#include <vector>
#include "solosnake/iimg_cache.hpp"

namespace solosnake
{
    class ifilefinder;

    class img_cache : public iimg_cache
    {
    public:
        
        explicit img_cache( const std::shared_ptr<ifilefinder>& );

        virtual ~img_cache();

        std::shared_ptr<iimgsrc> get_img_src( const std::string& ) override;

        //! Goes through the cache and attempts to unload or remove referenced
        //! images it considers to be not used. If you need to keep the cache
        //! available but want to free some memory space, calling this may help.
        void compact_cache() override;

        //! Returns the number of images currently held in the cache.
        size_t size() const;

    private:

        class cachedimgsrc;

        std::vector<std::shared_ptr<cachedimgsrc>>  cache_;
        std::shared_ptr<ifilefinder>                filefinder_;
    };
}

#endif
