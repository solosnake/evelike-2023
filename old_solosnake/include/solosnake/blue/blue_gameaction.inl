namespace blue
{
    inline size_t gameaction::bytes_size() const
    {
        return action_as_buffer_.size();
    }

    inline gameaction::gameaction() 
      : action_as_buffer_()
    {
    }

    inline const solosnake::network::bytebuffer& gameaction::to_bytebuffer() const
    {
        return action_as_buffer_;
    }

    inline gameaction::gameaction( const gameaction& rhs ) 
      : action_as_buffer_( rhs.action_as_buffer_ )
    {
    }

    inline gameaction& gameaction::operator=( const gameaction& rhs )
    {
        if( this != &rhs )
        {
            action_as_buffer_ = rhs.action_as_buffer_;
        }

        return *this;
    }

    inline gameaction::gameaction( gameaction&& rhs )
        : action_as_buffer_( std::move( rhs.action_as_buffer_ ) )
    {
    }

    inline gameaction& gameaction::operator=( gameaction && rhs )
    {
        if( this != &rhs )
        {
            action_as_buffer_ = std::move( rhs.action_as_buffer_ );
        }

        return *this;
    }

    inline gameaction::gameaction( std::vector<solosnake::network::byte>&& v )
        : action_as_buffer_( std::move( v ) )
    {
    }
}
