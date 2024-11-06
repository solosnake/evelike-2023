#include <memory>
#include "solosnake/modelnodedescription.hpp"

namespace solosnake
{
    modelnodedescription::modelnodedescription( std::string&& nodename,
            std::string&& meshname,
            const meshinstancedata& data )
        : node_name_( move( nodename ) ), mesh_name_( move( meshname ) ), instance_data_( data )
    {
    }

    modelnodedescription::modelnodedescription( modelnodedescription&& rhs )
        : node_name_( move( rhs.node_name_ ) )
        , mesh_name_( move( rhs.mesh_name_ ) )
        , instance_data_( rhs.instance_data_ )
        , childnodes_( move( rhs.childnodes_ ) )
    {
    }

    modelnodedescription& modelnodedescription::operator=( modelnodedescription && rhs )
    {
        if( this != &rhs )
        {
            node_name_ = move( rhs.node_name_ );
            mesh_name_ = move( rhs.mesh_name_ );
            instance_data_ = rhs.instance_data_;
            childnodes_ = move( rhs.childnodes_ );
        }

        return *this;
    }

    void modelnodedescription::add_child( modelnodedescription&& mnd )
    {
        childnodes_.push_back( std::move( mnd ) );
    }

    bool modelnodedescription::has_node_named( const std::string& name ) const
    {
        if( name == node_name_ )
        {
            return true;
        }
        else
        {
            for( size_t i = 0; i < childnodes_.size(); ++i )
            {
                if( childnodes_[i].has_node_named( name ) )
                {
                    return true;
                }
            }

            return false;
        }
    }
}
