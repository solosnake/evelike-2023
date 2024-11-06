#ifndef solosnake_modelscene_hpp
#define solosnake_modelscene_hpp

#include <map>
#include <vector>
#include "solosnake/deferred_renderer.hpp"

namespace solosnake
{
    //! Collects the instances of each model into arrays of common meshes.
    class modelscene
    {
    public:
        void add_mesh_instance( meshhandle_t, const meshinstancedata& );

        void render_scene( deferred_renderer& );

        //! Removes all added meshes.
        void clear_instances();

    private:
        typedef std::vector<meshinstancedata> instances_t;

        std::map<meshhandle_t, instances_t> contents_;
    };

    //-------------------------------------------------------------------------

    inline void modelscene::add_mesh_instance( meshhandle_t h, const meshinstancedata& data )
    {
        contents_[h].push_back( data );
    }
}

#endif
