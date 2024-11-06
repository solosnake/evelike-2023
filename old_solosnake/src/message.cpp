#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "solosnake/throw.hpp"
#include "solosnake/message.hpp"

namespace solosnake
{
    namespace network
    {
        message::message( const bytebuffer& s ) : body_length_( 0 )
        {
            contents_length( static_cast<unsigned int>( s.size() ) );
            memcpy( contents(), s.data(), contents_length() );
            encode_header();
        }

        bool message::decode_header()
        {
            using namespace std; // For strncat and atoi.
            char header[header_length + 1] = { byte( 0 ) };
            strncat( header, data_, header_length );
            const auto len = std::strtol( header, nullptr, 10 );
            if( len < 0 || len > max_body_length )
            {
                body_length_ = 0u;
                return false;
            }
            body_length_ = static_cast<unsigned int>( len );
            return true;
        }

        void message::encode_header()
        {
            using namespace std; // For sprintf and memcpy.
            char header[header_length + 1] = "";
            // As we are printing a number that we require to be 4 chars or less, 
            // the number needs to be less than 9999. In fact the decode will
            // not accept any number > max_body_length;
            static_assert( max_body_length <= 9999u, "4 character header length exceeded." );
            if( body_length_ <= max_body_length )
            {
              sprintf( header, "%4u", body_length_ );
            }
            else
            {
              sprintf( header, "%4u", 0u );
            }

            memcpy( data_, header, header_length );
        }

        void message::contents_length( unsigned int length )
        {
            body_length_ = length;
            if( body_length_ > max_body_length )
            {
                ss_throw( "Max message size exceeded." );
            }
        }
    }
}
