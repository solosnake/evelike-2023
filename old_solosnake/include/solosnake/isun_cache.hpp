#ifndef solosnake_isun_cache_hpp
#define solosnake_isun_cache_hpp

#include <string>
#include <memory>
#include "solosnake/isun_src.hpp"

namespace solosnake
{
    class imesh;

    //! An interface used by something which wishes load a sun from file and
    //! and which does not really care where the sun really comes from.
    class isun_cache : public isun_src
    {
    public:
        virtual ~isun_cache();

        virtual void load_sun_file( const SunType ) = 0;

        //! Attempts to free up space in the cache.
        virtual void compact_cache() = 0;
    };
}

#endif
