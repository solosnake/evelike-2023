#include <cassert>
#include <string>
#include "blue_version_cmake_generated.hpp"
#include "solosnake/blue/blue_get_buildstamp.hpp"

#ifndef SOLOSNAKE_STRINGIFY
#  define SOLOSNAKE_STRINGIFY(x)        #x
#  define SOLOSNAKE_TOSTRING(x)         SOLOSNAKE_STRINGIFY(x)
#endif

namespace blue
{
    //! Returns a string identifying the build.
    std::string get_buildstamp()
    {
        static const char buildstamp[] =
            BLUE_APP_MAJOR_MINOR_PATCH_VERSIONED_NAME ";"
            __DATE__ ";"
            __TIME__ ";"
#ifdef _DEBUG
            "_DEBUG" ";"
#endif
#ifdef NDEBUG
            "NDEBUG" ";"
#endif
#ifdef DEBUG
            "DEBUG" ";"
#endif
#ifdef _WIN64
            "_WIN64" ";"
#endif
#ifdef _WIN32
            "_WIN32" ";"
#endif
#ifdef __linux__
            "__linux__" ";"
#endif
#ifdef _UNICODE
            "_UNICODE" ";"
#else
            "NO-UNICODE" ";"
#endif
#ifdef _MSC_VER
            "_MSC_VER=" SOLOSNAKE_TOSTRING( _MSC_VER ) ";"
#endif
#ifdef WINVER
            "WINVER=" SOLOSNAKE_TOSTRING( WINVER ) ";"
#endif
#ifdef __GNUG__
            "__GNUG__"

#   ifdef __VERSION__
            __VERSION__
#   endif
            ";"
#endif
            ;

        return std::string( buildstamp ) + ( ( sizeof( void* ) == 4 ) ? "32-BIT;" : "64-BIT;" );
    }
}
