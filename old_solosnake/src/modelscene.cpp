#include <algorithm>
#include "solosnake/modelscene.hpp"

using namespace std;

namespace solosnake
{
    void modelscene::render_scene( deferred_renderer& r )
    {
        for_each( contents_.cbegin(),
                  contents_.cend(),
                  [&]( const map<meshhandle_t, instances_t>::value_type & meshes )
        {
            if( false == meshes.second.empty() )
            {
                r.draw_mesh_instances( meshes.first, &meshes.second[0], meshes.second.size() );
            }
        } );
    }

    void modelscene::clear_instances()
    {
        for_each( contents_.begin(),
                  contents_.end(),
                  [&]( map<meshhandle_t, instances_t>::value_type & meshes )
        { meshes.second.clear(); } );
    }
}
