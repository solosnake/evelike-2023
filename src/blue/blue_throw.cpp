#include "solosnake/blue/blue_throw.hpp"

ss_exception::ss_exception( char const * const m ) noexcept
    : std::runtime_error( m )
{
    // NOP
}
