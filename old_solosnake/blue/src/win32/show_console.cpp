#include <cstdio>
#include "solosnake/show_console.hpp"
#include "solosnake/win32/includewindows.hpp"

namespace solosnake
{
    void show_console()
    {
        ::AllocConsole();
        ::freopen( "conout$", "w", stdout );
        ::freopen( "conout$", "w", stderr );
    }
}
