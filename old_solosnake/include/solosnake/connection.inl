// Inline file

namespace solosnake
{
    namespace network
    {
        inline
        std::shared_ptr<connection>
        connection::make_shared( const std::shared_ptr<boost::asio::io_service>& s )
        {
            return std::make_shared<connection>( s, HeapOnly() );
        }


        inline
        void
        connection::rename( const std::string& txt )
        {
            assert( false == txt.empty() );
            assert( txt.size() == 1 );
            name_ = txt;
        }


        inline
        size_t
        connection::id() const
        {
            return id_;
        }


        inline
        const std::string&
        connection::name() const
        {
            return name_;
        }


        inline
        void
        connection::post_message( const bytebuffer& msg )
        {
            // Boost bind takes a copy of msg here.
            io_service_->post( 
                boost::bind( &connection::do_write, this, msg ) );
        }


        inline
        boost::asio::ip::tcp::socket&
        connection::socket()
        {
            return *socket_;
        }


        inline
        const boost::asio::ip::tcp::socket&
        connection::socket() const
        {
            return *socket_;
        }

    }
}
