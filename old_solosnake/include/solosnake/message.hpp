#ifndef solosnake_network_message_hpp
#define solosnake_network_message_hpp

#include <memory>
#include <string>
#include "byte.hpp"
#include "bytebuffer.hpp"

namespace solosnake
{
    namespace network
    {
        class message
        {
        public:

            enum Header
            {
                header_length = 4
            };

            enum Body
            {
                max_body_length = 512
            };

            message() : data_(), body_length_( 0 )
            {
            }

            message( const bytebuffer& s );

            const byte* package() const
            {
                return data_;
            }

            byte* package()
            {
                return data_;
            }

            unsigned int package_length() const
            {
                return header_length + body_length_;
            }

            const byte* contents() const
            {
                return data_ + header_length;
            }

            byte* contents()
            {
                return data_ + header_length;
            }

            unsigned int contents_length() const
            {
                return body_length_;
            }

            void contents_length( unsigned int length );

            bool decode_header();

            void encode_header();

        private:

            byte         data_[header_length + max_body_length];
            unsigned int body_length_;
        };
    }
}

#endif
