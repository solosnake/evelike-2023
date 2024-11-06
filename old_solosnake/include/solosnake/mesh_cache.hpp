#ifndef solosnake_mesh_cache_hpp
#define solosnake_mesh_cache_hpp

#include <map>
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/imesh_cache.hpp"

namespace solosnake
{
    //! Meshes loaded from this will also reload themselves via this cache.
    //! Loaded meshes do not require this object to remain in scope - they are
    //! safely independent of the lifetime of this object.
    class mesh_cache : public imesh_cache
    {
    public:

        mesh_cache(
            const std::shared_ptr<ifilefinder>& meshes,
            const std::shared_ptr<ifilefinder>& textures );

        virtual ~mesh_cache();

        void load_meshes_file( const std::string& ) override;

        std::shared_ptr<imesh> get_imesh( const std::string& ) override;

        //! Goes through the cache and attempts to unload or remove referenced
        //! meshes it considers to be not used. If you need to keep the cache
        //! available but want to free some memory space, calling this may help.
        void compact_cache() override;

        //! Returns the number of meshes currently held in the cache.
        size_t size() const;

    private:

        class mesh_impl;
        std::shared_ptr<mesh_impl> impl_;
    };
}

#endif
