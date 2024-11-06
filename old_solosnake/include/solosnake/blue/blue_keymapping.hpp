#ifndef blue_keymapping_hpp
#define blue_keymapping_hpp

#include <map>
#include <memory>
#include "solosnake/blue/blue_gamecommands.hpp"

namespace blue
{
    //! Contains the mappings of keys to commands.
    class keymapping
    {
    public:

        void map_key_to(size_t vkey, GameCommands cmd);

        void remove_mapping_for(size_t vkey);

        bool find(size_t vkey, GameCommands& cmd) const;

    private:

        std::map<size_t, GameCommands> mapping_;
    };

}

#endif
