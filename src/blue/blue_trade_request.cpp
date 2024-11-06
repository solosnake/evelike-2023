#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_trade_request.hpp"

namespace blue
{
    bool operator == (const Trade_request& , const Trade_request& ) noexcept
    {
        ss_throw("Not implemented");
    }
}
