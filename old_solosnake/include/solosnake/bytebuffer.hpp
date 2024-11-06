#ifndef solosnake_bytebuffer_hpp
#define solosnake_bytebuffer_hpp

#include <string>
#include <vector>
#include "solosnake/byte.hpp"

namespace solosnake
{
    namespace network
    {
        class bytebuffer
        {
        public:

            bytebuffer();

            bytebuffer( const bytebuffer& );

            explicit bytebuffer( const std::string& );

            explicit bytebuffer( const size_t ncount );

            explicit bytebuffer( std::vector<byte>&& );

            explicit bytebuffer( const byte*, size_t ncount );

            bytebuffer( bytebuffer&& );

            bytebuffer& operator=( const bytebuffer& );

            bytebuffer& operator=( bytebuffer && );

            size_t size() const;

            const byte* data() const;

            byte* data();

            void clear();

            void resize( const size_t n );

            bool is_empty() const;

            bool operator<( const bytebuffer& rhs ) const;

            bool operator==( const bytebuffer& rhs ) const;

            bytebuffer& operator+=( const bytebuffer& );

            bytebuffer& operator+=( const byte );

        private:
            std::vector<byte> data_;
        };
    }
}

#include "solosnake/bytebuffer.inl"

#endif
