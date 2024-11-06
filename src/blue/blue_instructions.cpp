
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Instructions::Instructions( std::vector<Instruction>&& other )
        : instructions_( std::move( other ) )
    {
        if( instructions_.empty() )
        {
            ss_throw( "Constructing instructions with no instructions." );
        }
        else if( instructions_.size() > BLUE_MAX_BOT_INSTRUCTIONS )
        {
            ss_throw( "Constructing instructions with too many instructions." );
        }
    }

    Instructions::Instructions( const std::vector<Instruction>& v )
        : instructions_( v )
    {
        if( instructions_.empty() )
        {
            ss_throw( "Constructing instructions with no instructions." );
        }
        else if( instructions_.size() >= BLUE_MAX_BOT_INSTRUCTIONS )
        {
            ss_throw( "Too many instructions." );
        }
    }

    Instructions::Instructions( const std::string_view& code )
        : Instructions( blue::compile_code(code) )
    {
        if( instructions_.empty() )
        {
            ss_throw( "Constructing instructions with no instructions." );
        }
        else if( instructions_.size() >= BLUE_MAX_BOT_INSTRUCTIONS )
        {
            ss_throw( "Too many instructions." );
        }
    }
}