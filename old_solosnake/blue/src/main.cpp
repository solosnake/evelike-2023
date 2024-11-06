#include <exception>
#include <iostream>
#include "solosnake/logging.hpp"
#include "solosnake/blue/blue_main.hpp"

//! Application entry point - this may be called by WinMain. Runs blue and attempts to catch
//! and log any exceptions throw before terminating the application. Unknown exceptions are
//! not caught.
int main( int argc, char** argv )
{
    int result = EXIT_FAILURE;

    try
    {
        result = blue_main( argc, argv );
    }
    catch( const std::exception& e )
    {
        ss_err( e.what() );
    }

    return result;
}
