#include <string>
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_compilation_result.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Compilation_result, Compilation_result )
{
    Compilation_result succeeded( Instruction::nop() );
    EXPECT_TRUE( succeeded.was_successful() );

    Compilation_result failed( "SOMETHING BAD" );
    EXPECT_FALSE( failed.was_successful() );
    EXPECT_TRUE( failed.error_text() == "SOMETHING BAD" );

    EXPECT_THROW( Compilation_result( "" ) );
}
