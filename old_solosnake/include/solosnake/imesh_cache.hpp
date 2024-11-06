#ifndef solosnake_imesh_cache_hpp
#define solosnake_imesh_cache_hpp

#include <string>
#include <memory>
#include "solosnake/imesh_src.hpp"

namespace solosnake
{
    class imesh;

    //! An interface used by something which wishes load meshes from file and
    //! to retrieve a mesh via its name but which does not really care where
    //! the mesh really comes from.
    //! A mesh is a single block of related geometry and textures.
    class imesh_cache : public imesh_src
    {
    public:
        virtual void load_meshes_file( const std::string& ) = 0;

        /// Attempts to free up space in the cache.
        virtual void compact_cache() = 0;

        virtual ~imesh_cache();
    };

    typedef std::shared_ptr<imesh_cache> imesh_cache_ptr;
}

#endif
