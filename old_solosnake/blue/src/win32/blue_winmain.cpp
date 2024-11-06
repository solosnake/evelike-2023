#include <iostream>
#include "solosnake/logging.hpp"
#include "solosnake/win32/includewindows.hpp"
#include "solosnake/win32/exception_code_description.hpp"

extern int main( int argc, char** argv );

namespace
{
    //! Tries to log details of what went wrong and always returns EXCEPTION_EXECUTE_HANDLER.
    int logStructuredException( const DWORD error )
    {
        const auto e = solosnake::get_exceptioncode_description( error );
        return EXCEPTION_EXECUTE_HANDLER;
    }
}

//! Windows entry point that calls into ANSI C++ main function.
int CALLBACK WinMain( __in HINSTANCE, __in_opt HINSTANCE, __in LPSTR, __in int )
{
#ifdef NDEBUG
    __try
    {
        return main( __argc, __argv );
    }
    __except( logStructuredException( ::GetExceptionCode() ) )
    {
        return EXIT_FAILURE;
    }
#else
    return main( __argc, __argv );
#endif
}
