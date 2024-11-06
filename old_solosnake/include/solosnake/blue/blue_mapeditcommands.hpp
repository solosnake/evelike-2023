#ifndef blue_mapeditcommands_hpp
#define blue_mapeditcommands_hpp

#include "solosnake/commandpattern.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_mapedittarget.hpp"

namespace blue
{
    typedef solosnake::command<mapedittarget>   mapeditcommand_t;

    //! Class capturing the action that changes a hexgrid tile. This can remove tiles.
    class changetile : public solosnake::command<mapedittarget>
    {
    public:

        explicit changetile( const Hex_coord xy, const HexContentFlags f );

        std::unique_ptr<mapeditcommand_t> execute_command_on( mapedittarget& t ) override;

    private:

        Hex_coord            xy_;
        HexContentFlags     flags_;
    };
}

#endif
