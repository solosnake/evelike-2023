#include <cstring>
#include "solosnake/bytebuffer.hpp"

namespace solosnake
{
    namespace network
    {
        bytebuffer::bytebuffer( const byte* data, size_t ncount ) : data_( data, data + ncount )
        {
        }

        bytebuffer::bytebuffer( const std::string& s ) : data_( s.c_str(), s.c_str() + s.length() + 1 )
        {
        }

        bool bytebuffer::operator<( const bytebuffer& rhs ) const
        {
            return size() < rhs.size() || ( size() == rhs.size() && !is_empty()
                                            && std::memcmp( data(), rhs.data(), size() ) < 0 );
        }
    }
}