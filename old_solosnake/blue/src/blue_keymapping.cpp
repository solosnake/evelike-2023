#include "solosnake/blue/blue_keymapping.hpp"

namespace blue
{
    void keymapping::map_key_to( size_t vkey, GameCommands cmd )
    {
        mapping_[vkey] = cmd;
    }

    void keymapping::remove_mapping_for( size_t vkey )
    {
        mapping_.erase( mapping_.find( vkey ) );
    }

    //! Returns true if the key is found and returns the associated command,
    //! else returns false.
    bool keymapping::find( size_t vkey, GameCommands& cmd ) const
    {
        auto i = mapping_.find( vkey );

        if( i != mapping_.end() )
        {
            cmd = i->second;
            return true;
        }

        return false;
    }
}
