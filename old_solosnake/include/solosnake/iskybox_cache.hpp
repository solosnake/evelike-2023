#ifndef solosnake_iskybox_cache_hpp
#define solosnake_iskybox_cache_hpp

#include <string>
#include <memory>
#include "solosnake/iskybox_src.hpp"

namespace solosnake
{
    //! An interface used by something which wishes load a skybox from file and
    //! and which does not really care where the skybox really comes from.
    class iskybox_cache : public iskybox_src
    {
    public:
        virtual ~iskybox_cache();

        virtual void load_skybox_file( const std::string& ) = 0;

        //! Attempts to free up space in the cache.
        virtual void compact_cache() = 0;
    };
}

#endif
