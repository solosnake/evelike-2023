#include <iostream>
#include <deque>
#include <limits>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "solosnake/connection.hpp"

using namespace boost;

namespace solosnake
{
    namespace network
    {
        namespace
        {
            //! Static function passed to asio as callback to close
            //! connection's socket.
            void close_socket( const std::shared_ptr<boost::asio::ip::tcp::socket>& s )
            {
                if( s )
                {
                    if( s->is_open() )
                    {
                        s->shutdown( asio::ip::tcp::socket::shutdown_both );
                        s->close();
                    }
                }
            }
        }

        //! Heap only ctor.
        connection::connection( const std::shared_ptr<asio::io_service>& service, const HeapOnly& )
            : io_service_( service )
            , socket_( std::make_shared<boost::asio::ip::tcp::socket>( *service ) )
            , id_( std::numeric_limits<size_t>::max() )
        {
            assert( service );
        }

        connection::~connection()
        {
            if( io_service_ && socket_ )
            {
                if( socket_->is_open() )
                {
                    close();
                }
            }
        }

        void connection::close()
        {
            io_service_->post(
                boost::bind( &close_socket, socket_ ) );
        }

        void connection::do_write( const bytebuffer& msg )
        {
            const bool write_in_progress = ( false == messages_to_post_.empty() );

            messages_to_post_.push_back( message( msg ) );

            if( false == write_in_progress )
            {
                asio::async_write(
                    *socket_,
                    asio::buffer( messages_to_post_.front().package(),
                                  messages_to_post_.front().package_length() ),
                    boost::bind( &connection::handle_write, this, asio::placeholders::error ) );
            }
        }

        void connection::handle_write( const boost::system::error_code& error )
        {
            if( ! error )
            {
                // std::clog << "Wrote message to " << socket_.remote_endpoint() <<
                // std::endl;

                messages_to_post_.pop_front();

                if( false == messages_to_post_.empty() )
                {
                    asio::async_write(
                        *socket_,
                        asio::buffer( messages_to_post_.front().package(),
                                      messages_to_post_.front().package_length() ),
                        boost::bind( &connection::handle_write, this, asio::placeholders::error ) );
                }
            }
            else
            {
                close();
            }
        }

        void connection::nagle( bool onoff )
        {
            asio::ip::tcp::no_delay option( onoff );
            socket_->set_option( option );
        }

        // Writes the connection to the stream in a human readable format.
        std::ostream& operator<<( std::ostream& os, const connection& c )
        {
            os << "'" << c.name().c_str();

            boost::system::error_code ecLocal;
            boost::system::error_code ecRemote;
            auto slocal  = c.socket().local_endpoint( ecLocal );
            auto sremote = c.socket().remote_endpoint( ecRemote );

            os << "', open = " << ( c.socket().is_open() ? "true" : "false" );

            if( ecLocal )
            {
                os << ", no local endpoint";
            }
            else
            {
                os << ", local endpoint = " << slocal;
            }

            if( ecRemote )
            {
                os << ", no remote endpoint, ";
            }
            else
            {
                os << ", remote endpoint = " << sremote;
            }

            return os;
        }
    }
}
