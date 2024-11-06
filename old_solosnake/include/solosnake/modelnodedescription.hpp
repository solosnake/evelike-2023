#ifndef solosnake_modelnodedescription_hpp
#define solosnake_modelnodedescription_hpp

#include <vector>
#include <string>
#include "solosnake/matrix3d.hpp"
#include "solosnake/deferred_renderer.hpp"

namespace solosnake
{
    class modelnodedescription
    {
    public:
        modelnodedescription();

        modelnodedescription( std::string&& nodename, std::string&& meshname, const meshinstancedata& );

        modelnodedescription( modelnodedescription&& );

        modelnodedescription& operator=( modelnodedescription && );

        void add_child( modelnodedescription&& mnd );

        bool has_node_named( const std::string& name ) const;

        const std::string& node_name() const
        {
            return node_name_;
        }

        const std::string& mesh_name() const
        {
            return mesh_name_;
        }

        size_t child_node_count() const
        {
            return childnodes_.size();
        }

        const modelnodedescription& child_node( size_t n ) const
        {
            return childnodes_[n];
        }

        const meshinstancedata& instance_data() const
        {
            return instance_data_;
        }

    private:
        std::string node_name_;
        std::string mesh_name_;
        meshinstancedata instance_data_;
        std::vector<modelnodedescription> childnodes_;
    };
}

#endif
