#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_logging.hpp"

TEST(logging, logging)
{
    ss_log( "This is a test logging" );
    ss_wrn( "This is a test warning" );
    ss_err( "This is a test error" );
}
