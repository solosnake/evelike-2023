#ifndef solosnake_iimg_cache_hpp
#define solosnake_iimg_cache_hpp

#include <memory>
#include <string>

namespace solosnake
{
    class iimgsrc;

    //! An interface used by something which wishes load images from file and
    //! to retrieve a img via its name but which does not really care where
    //! the img really comes from.
    //! A img is a single block of related geometry and textures.
    class iimg_cache
    {
    public:

        virtual ~iimg_cache();

        virtual std::shared_ptr<iimgsrc> get_img_src( const std::string& ) = 0;

        //! Attempts to free up space in the cache.
        virtual void compact_cache() = 0;
    };
}

#endif
