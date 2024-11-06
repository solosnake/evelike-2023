#ifndef solosnake_byte_hpp
#define solosnake_byte_hpp

#include <cstdint>

namespace solosnake
{
    namespace network
    {
        typedef char byte;
    }

    static_assert( sizeof( char ) == 1, "c++ char type is required to be 8 bits." );
}

#endif
