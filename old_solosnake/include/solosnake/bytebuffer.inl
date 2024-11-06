#include <algorithm>

namespace solosnake
{
    namespace network 
    {
        inline bytebuffer::bytebuffer() : data_()
        {
        }

        inline bytebuffer::bytebuffer( const size_t ncount ) : data_( ncount, byte( 0 ) )
        {
        }

        inline size_t bytebuffer::size() const
        {
            return data_.size();
        }

        inline const byte* bytebuffer::data() const
        {
            return data_.data();
        }

        inline byte* bytebuffer::data()
        {
            return data_.data();
        }

        inline void bytebuffer::clear()
        {
            data_.clear();
        }

        inline void bytebuffer::resize( const size_t n )
        {
            data_.resize( n );
        }

        inline bool bytebuffer::is_empty() const
        {
            return data_.empty();
        }

        inline bool bytebuffer::operator==( const bytebuffer& rhs ) const
        {
            return data_ == rhs.data_;
        }

        inline bytebuffer::bytebuffer( const bytebuffer& rhs ) : data_( rhs.data_ )
        {
        }

        inline bytebuffer::bytebuffer( bytebuffer&& rhs ) : data_( std::move( rhs.data_ ) )
        {
        }

        inline bytebuffer::bytebuffer( std::vector<byte>&& v ) : data_( std::move( v ) )
        {
        }

        inline bytebuffer& bytebuffer::operator=( const bytebuffer& rhs )
        {
            if( this != &rhs )
            {
                data_ = rhs.data_;
            }

            return *this;
        }

        inline bytebuffer& bytebuffer::operator=( bytebuffer && rhs )
        {
            if( this != &rhs )
            {
                data_ = std::move( rhs.data_ );
            }

            return *this;
        }

        inline bytebuffer& bytebuffer::operator+=( const bytebuffer& rhs )
        {
            const auto oldSize = data_.size();
            data_.resize( oldSize + rhs.size() );
            std::copy( rhs.data_.cbegin(), rhs.data_.cend(), data_.begin() + oldSize );
            return *this;
        }

        inline bytebuffer& bytebuffer::operator+=( byte b )
        {
            data_.push_back( b );
            return *this;
        }
    }
}
