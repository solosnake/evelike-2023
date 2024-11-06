#include "solosnake/blue/blue_compilation_result.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Compilation_result::Compilation_result( const std::string_view& err )
        : error_( err )
        , instruction_( Instruction::nop() )
    {
        if( err.empty() )
        {
            ss_throw("Erroneous compilation result text cannot be empty");
        }
    }

    const Instruction& Compilation_result::get_instruction() const
    {
        if( was_successful() )
        {
            return instruction_;
        }
        else
        {
            ss_err( error_ );
            ss_throw( "Compilation was not successful." );
        }
    }
}
