#ifndef solosnake_connection_hpp
#define solosnake_connection_hpp

#include <deque>
#include <memory>
#include <iosfwd>
#include <boost/asio.hpp>
#include "solosnake/message.hpp"
#include "solosnake/bytebuffer.hpp"

namespace solosnake
{
    namespace network
    {
        class connection
            : public std::enable_shared_from_this<connection>
        {
            struct HeapOnly { }; // Restricts availability of ctor.

        public:

            static std::shared_ptr<connection> make_shared(
                const std::shared_ptr<boost::asio::io_service>& s );

            //! Dtor closes connection
            ~connection();

            void rename( const std::string& txt );

            size_t id() const;

            const std::string& name() const;

            void post_message( const bytebuffer& msg );

            //! See http://en.wikipedia.org/wiki/Nagle%27s_algorithm
            void nagle( bool onoff );

            friend std::ostream& operator<<( std::ostream&, const connection& );

            const boost::asio::ip::tcp::socket& socket() const;

            boost::asio::ip::tcp::socket& socket();

        public:

            //! Ctor is public but restricted by HeapOnly being private.
            connection(
                const std::shared_ptr<boost::asio::io_service>&,
                const connection::HeapOnly& );

        private:

            void close();

            void do_write( const bytebuffer& msg );

            void handle_write( const boost::system::error_code& error );

        private:

            connection( const connection& ); // = delete;
            connection& operator=( const connection& ); // = delete;

        private:

            std::shared_ptr<boost::asio::io_service>        io_service_;
            std::shared_ptr<std::string>                    incoming_msg_;
            std::shared_ptr<boost::asio::ip::tcp::socket>   socket_;
            std::deque<message>                             messages_to_post_;
            std::string                                     name_;
            size_t                                          id_;
        };

        //-------------------------------------------------------------------------

        //! Writes the network connection to the stream in a human readable format.
        std::ostream& operator<<( std::ostream&, const connection& );
    }
}

#include "solosnake/connection.inl"

#endif
