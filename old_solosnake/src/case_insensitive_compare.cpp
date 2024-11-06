#include <locale>
#include "solosnake/case_insensitive_compare.hpp"

namespace solosnake
{
    bool case_insensitive_compare( const char * const lhs, const char* const rhs )
    {
        if( lhs == nullptr && rhs == nullptr )
        {
            return true;
        }
        else if( (lhs != nullptr && rhs == nullptr) || (lhs == nullptr) ) 
        {
            // Note: static analysis showed that there is no need for the
            // check (lhs == nullptr && rhs != nullptr) as the 2nd condition never 
            // occurs.
            return false;
        }
        else
        {
            const std::locale loc;
            bool same = true;
            bool text_remaining = true;
            const char * plhs = lhs;
            const char * prhs = rhs;

            while( text_remaining && same )
            {
                const auto lowerlhs = std::tolower( *plhs, loc );
                const auto lowerrhs = std::tolower( *prhs, loc );
                text_remaining = (lowerlhs != 0 && lowerrhs != 0);
                same           = (lowerlhs == lowerrhs);
                ++plhs;
                ++prhs;
            }

            return same;
        }
    }
}
