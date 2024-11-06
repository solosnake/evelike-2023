#include "solosnake/blue/blue_mapeditcommands.hpp"

namespace blue
{
    changetile::changetile( const Hex_coord xy, const HexContentFlags f ) : xy_( xy ), flags_( f )
    {
    }

    std::unique_ptr<mapeditcommand_t> changetile::execute_command_on( mapedittarget& t )
    {
        auto oldflags = t.grid_.contents( xy_ );
        t.grid_.set_contents( xy_, flags_ );
        return std::make_unique<changetile>( xy_, oldflags );
    }
}
