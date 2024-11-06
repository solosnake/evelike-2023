#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "solosnake/lockstepserver.hpp"
#include "solosnake/connection.hpp"
#include "solosnake/nothrow.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

//#define SS_NO_LOCKSTEP_LOG
using namespace std;

// TODO: Can this conditional compilation be moved into CMake or out of code somehow?
#ifdef _MSC_VER
#   pragma warning(disable : 4503) // warning C4503 : decorated name length exceeded, name was truncated
#endif

namespace solosnake
{
    namespace network
    {
        lockstepserver::exception::exception( const char* w ) : SS_EXCEPTION_TYPE( w )
        {
        }

        namespace
        {
            namespace MsgCommands
            {
                const size_t Length = 8;

                const char NameSeperator = '+';
                const char PortSeperator = ':';

                namespace ToHost
                {
                    //                         01234567
                    const char PortOpen[9]  = "PORTOPEN";
                    const char Connected[9] = "CONNCTED";
                    const char Innited[9]   = "INNITED ";
                }

                namespace ToClient
                {
                    //                        01234567
                    const char Connect[9]  = "CONNECT ";
                    const char OpenPort[9] = "OPENPORT";
                    const char Begin[9]    = "BEGIN   ";
                    const char Init[9]     = "INIT    ";
                }
            }

            enum LockstepServerErrorType
            {
                UnknownMessage,
                InvalidCmdPacket,
                UnexpectedMessage, //!< A message was received, known or unknown,which was unexpected for the current state.
                UnexpectedMessageLength, //!< A message had an unusual length that could not be decoded, probably too short.
                UnableToDecodePacketHeader,
                UnableToOpenPort,
                NameAlreadySet,
                InvalidCommand,
                LogicError,
                ConnectionTimeout,
                MissingEndpoint
            };

            void loop_post_message_to_connection( shared_ptr<connection> p, const bytebuffer& s )
            {
                p->post_message( s );
            }

            //! Prints @a b as ASCII if it is printable, else as hex.
            void print_byte( const byte b, ostream& os )
            {
                const byte ascii_printable_begin = 0x20;
                const byte ascii_printable_end   = 0x7E;

                if( b >= ascii_printable_begin && b <= ascii_printable_end )
                {
                    os << ' ' << b << ' ';
                }
                else
                {
                    os << hex << int( b + 128 ) << dec << ' ';
                }
            }
        }

        // -------------- boost error messages --------------

        class lockstepserver_error_category : public boost::system::error_category
        {
        public:

            lockstepserver_error_category()
            {
            }

            string message( int ev ) const override
            {
                switch( ev )
                {
                    case UnknownMessage:
                        return "Unknown Message. A message was received which was not "
                               "understood.";

                    case InvalidCmdPacket:
                        return "The packet associated with a received command was "
                               "invalid.";

                    case UnexpectedMessage:
                        return "Unexpected Message. A message was received, known or "
                               "unknown, which was unexpected for the current state.";

                    case UnexpectedMessageLength:
                        return "Unexpected Message Length. A message was received that "
                               "appeared to be the wrong length, probably too short.";

                    case UnableToDecodePacketHeader:
                        return "Unable to decode message packet header.";

                    case UnableToOpenPort:
                        return "Unable to open a port.";

                    case NameAlreadySet:
                        return "Client has already had name set.";

                    case InvalidCommand:
                        return "The command string received was invalid.";

                    case LogicError:
                        return "The application reached an unexpected state due to a "
                               "bug.";

                    case ConnectionTimeout:
                        return "Application timed out waiting for connections to be "
                               "made.";

                    case MissingEndpoint:
                        return "Querying socket for an endpoint failed.";

                    default:
                        assert( ! "Forgot to update error categories." );
                        return "Unknown Error";
                }
            }

            const char* name() const SS_NOEXCEPT override
            {
                return "lockstepserver";
            }
        };

        // --------------- Lock-step server implementation ---------------

        class lockstepserverImpl
        {
        public:

            lockstepserverImpl(
                const shared_ptr<lockstepserver::inetworkgameloop>&,
                const size_t expected_participants_count,
                const unsigned short port,
                const string& log,
                const unsigned long tickTimeLengthMs,
                const unsigned short timeout_limit_s );

            lockstepserverImpl(
                const shared_ptr<lockstepserver::inetworkgameloop>&,
                const string& ipaddress,
                const unsigned short port,
                const string& log,
                const unsigned long tickTimeLengthMs,
                const unsigned short timeout_limit_s );

            size_t connections_count() const
            {
                return connections_.size();
            }

            size_t players_count() const
            {
                return 1 + connections_count();
            }

            virtual ~lockstepserverImpl();

            void insert_message( const bytebuffer& );

        private:

            lockstepserverImpl( const lockstepserverImpl& ); // = delete;
            lockstepserverImpl& operator= ( const lockstepserverImpl& ); // = delete;

        private:

            typedef void ( lockstepserverImpl::*CommandFunctionPtr )( const string& );
            typedef boost::asio::ip::tcp::acceptor            tcpacceptor;
            typedef boost::asio::ip::tcp::endpoint            tcpendpoint;
            typedef map<string, CommandFunctionPtr>           command_function_map;
            typedef shared_ptr<boost::asio::io_service::work> work_sptr;
            typedef map<string, bytebuffer>                   message_map;
            typedef vector<string>                            string_array;
            typedef shared_ptr<string_array>                  string_array_ptr;
            typedef vector<shared_ptr<connection>>            connection_sptr_array;
            typedef shared_ptr<connection_sptr_array>         connection_sptr_array_sptr;

        private:

            void run_service(
                const shared_ptr<lockstepserver::inetworkgameloop>& );

            void loop_get_frame_update_messages_from_clients();

            void loop_post_listen_for_message(
                const shared_ptr<connection>& );

            void loop_game_message_read_header(
                const shared_ptr<connection>&,
                const shared_ptr<message>&,
                const boost::system::error_code& );

            void loop_game_message_read_body(
                const shared_ptr<connection>&,
                const shared_ptr<message>&,
                const boost::system::error_code& );

            bool waiting_for_more_messages() const;

            string this_player_name() const;

            vector<string> other_player_names() const;

            string generate_connection_name(
                const size_t ) const;

            string get_str_port(
                unsigned short ) const;

            unsigned short get_port_from_str(
                const string& ) const;

            string socket_address(
                const boost::asio::ip::tcp::socket& ) const;

            string endpoint_address(
                const boost::asio::ip::tcp::endpoint& ) const;

            shared_ptr<tcpacceptor> create_acceptor( const unsigned short );

            boost::system::error_code find_socket_port(
                const shared_ptr<connection>&,
                unsigned short& ) const;

            void open_log(
                const string& );

            void validate(
                const size_t ) const;

            void validate(
                const shared_ptr<lockstepserver::inetworkgameloop>& ) const;

            unsigned short local_port() const;

            void log_socket_connectivity() const;

            void log_client_messages() const;

            void log_message(
                const byte*,
                const size_t length ) const;

            void add_connection(
                const shared_ptr<connection>& );

            void close_all_sockets();

            void signal_error(
                const boost::system::error_code& );

            void signal_error(
                LockstepServerErrorType );

            bool is_expected_nothrow_socket_closing_error(
                const boost::system::error_code& ) const;

            bool is_valid_client_name(
                const string& ) const;

            void master_accept_new_client(
                const shared_ptr<connection>&,
                const shared_ptr<tcpacceptor>&,
                const shared_ptr<tcpendpoint>&,
                const boost::system::error_code& );

            void master_init_connection( size_t );

            void master_init_connections();

            void master_begin_game();

            void master_make_connections();

            void master_make_connections_and_begin_game();

            void master_connect_to_other_clients_or_begin_game( size_t );

            void master_connect_client_to_indexed_client(
                const size_t,
                const size_t,
                const connection_sptr_array_sptr& );

            void master_handle_reply_to_init_header(
                const shared_ptr<message>&,
                const size_t,
                const boost::system::error_code& );

            void master_handle_reply_to_init_body(
                const shared_ptr<message>&,
                const size_t,
                const boost::system::error_code& );

            void master_handle_reply_to_open_port_header(
                const shared_ptr<message>&,
                const size_t,
                const size_t,
                const connection_sptr_array_sptr&,
                const boost::system::error_code& );

            void master_handle_reply_to_open_port_body(
                const shared_ptr<message>&,
                const size_t,
                const size_t,
                const connection_sptr_array_sptr&,
                const boost::system::error_code& );

            void master_handle_reply_to_connect_header(
                const shared_ptr<message>&,
                const size_t,
                const size_t,
                const connection_sptr_array_sptr&,
                const boost::system::error_code& );

            void master_handle_reply_to_connect_body(
                const shared_ptr<message>&,
                const size_t,
                const size_t,
                const connection_sptr_array_sptr&,
                const boost::system::error_code& );

            void client_build_function_map();

            void client_cmd_connect( const string& );

            void client_cmd_openport( const string& );

            void client_cmd_begin( const string& );

            void client_cmd_init( const string& );

            void client_wait_for_messages();

            void client_process_message( const string& msg );

            void client_connect_to_server(
                const string& ipaddress,
                const string& port );

            void client_make_connection_to_server(
                const shared_ptr<connection>&,
                const boost::system::error_code&,
                boost::asio::ip::tcp::resolver::iterator );

            void client_make_connection_to_client(
                const shared_ptr<connection>&,
                const boost::system::error_code&,
                boost::asio::ip::tcp::resolver::iterator );

            void client_accept_new_client(
                const shared_ptr<connection>&,
                const shared_ptr<tcpacceptor>&,
                const shared_ptr<tcpendpoint>&,
                const boost::system::error_code& );

            void client_handle_read_master_message_header(
                const shared_ptr<message>&,
                const boost::system::error_code& );

            void client_handle_read_master_message_body(
                const shared_ptr<message>&,
                const boost::system::error_code& );

            string client_extract_name( const string& );

            size_t client_extract_participants_count( const string& );

        private:

            string                                  name_;
            shared_ptr<boost::asio::io_service>     io_service_;
            mutable boost::mutex                    mut_;
            connection_sptr_array                   connections_;
            command_function_map                    functions_;
            message_map                             client_messages_;
            bytebuffer                              local_message_;
            const bytebuffer                        nop_local_message_;
            size_t                                  expected_participants_count_;
            ostream*                                log_;
            ofstream                                logfile_;
            unsigned long                           timeout_limit_ms_;
            unsigned long                           duration_of_gametick_ms_;
            unsigned short                          master_port_;
            volatile bool                           ready_to_run_loop_;
            volatile bool                           error_free_;
            volatile bool                           connection_ok_;
            volatile bool                           closing_all_sockets_;
            boost::system::error_code               error_;
        };

        // ----------------- Master only code -----------------

        //! Constructor for Master server object.
        //! The master server is responsible for waiting for expected number of
        // clients to connect
        //! and then informing each one of the others before the game begins.
        lockstepserverImpl::lockstepserverImpl(
            const shared_ptr<lockstepserver::inetworkgameloop>& pgame,
            const size_t expected_participants_count,
            const unsigned short port,
            const string& logfilename,
            const unsigned long tickTimeLengthMs,
            const unsigned short timeout_limit_s )
            : io_service_( new boost::asio::io_service() )
            , nop_local_message_()
            , expected_participants_count_( expected_participants_count )
            , log_( NULL )
            , timeout_limit_ms_( static_cast<long>( timeout_limit_s * 1000 ) )
            , duration_of_gametick_ms_( tickTimeLengthMs )
            , master_port_( port )
            , ready_to_run_loop_( expected_participants_count < 2 )
            , error_free_( true )
            , connection_ok_( true )
            , closing_all_sockets_( false )
        {
            name_ = generate_connection_name( expected_participants_count_ );

            open_log( logfilename );

            validate( expected_participants_count );
            validate( pgame );

            try
            {
                if( log_ )
                {
                    *log_ << "Constructing master for " << expected_participants_count << " at port "
                          << port << endl;
                }

                if( expected_participants_count_ > 1 )
                {
                    connections_.reserve( expected_participants_count_ - 1 );

                    shared_ptr<connection> server = connection::make_shared( io_service_ );

                    server->rename( name_ );

                    shared_ptr<tcpacceptor> acptr = create_acceptor( master_port_ );

                    if( acptr )
                    {
                        // This exists purely for convenience of logging. It
                        // allows us to capture the endpoint of the acceptor,
                        // as the sockets seem to take a slightly more ambiguous
                        // view of which end is the 'end point' which can make
                        // the logs confusing.
                        shared_ptr<tcpendpoint> acceptorendpoint( new boost::asio::ip::tcp::endpoint() );

                        acptr->async_accept( server->socket(),
                                             *acceptorendpoint,
                                             boost::bind( &lockstepserverImpl::master_accept_new_client,
                                                          this,
                                                          server,
                                                          acptr,
                                                          acceptorendpoint,
                                                          boost::asio::placeholders::error ) );
                    }
                }

                run_service( pgame );
            }
            catch( const lockstepserver::exception& e )
            {
                if( log_ )
                {
                    *log_ << e.what() << endl;
                }
                throw;
            }
            catch( const exception& e )
            {
                if( log_ )
                {
                    *log_ << e.what() << endl;
                }
                throw lockstepserver::exception( e.what() );
            }
            catch( ... )
            {
                throw lockstepserver::exception( "Unknown exception." );
            }
        }

        void lockstepserverImpl::master_accept_new_client(
            const shared_ptr<connection>& client,
            const shared_ptr<tcpacceptor>& acptr,
            const shared_ptr<tcpendpoint>& acceptorendpoint,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                if( log_ )
                {
                    *log_ << "Master accepted new client connection; endpoint "
                          "after accept is " << acceptorendpoint->address() << ":"
                          << ":" << acceptorendpoint->port()
                          << ", connection is " << *client << endl;
                }

                add_connection( client );

                if( players_count() < expected_participants_count_ )
                {
                    shared_ptr<connection> newclient = connection::make_shared( io_service_ );
                    acptr->async_accept( newclient->socket(),
                                         *acceptorendpoint,
                                         boost::bind( &lockstepserverImpl::master_accept_new_client,
                                                      this,
                                                      newclient,
                                                      acptr,
                                                      acceptorendpoint, // re-use this.
                                                      boost::asio::placeholders::error ) );
                }
                else if( players_count() == expected_participants_count_ )
                {
                    if( log_ )
                    {
                        *log_ << "Expected participants count (" << expected_participants_count_
                              << ") met." << endl;
                    }

                    master_init_connections();
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_init_connections()
        {
            master_init_connection( 0 );
        }

        void lockstepserverImpl::master_init_connection( size_t index )
        {
            if( index == connections_.size() )
            {
                if( log_ )
                {
                    *log_ << "All connections are named." << endl;
                }

                master_make_connections_and_begin_game();
            }
            else
            {
                shared_ptr<connection> c = connections_.at( index );

                c->rename( generate_connection_name( index ) );

                shared_ptr<message> listenerMsg( new message() );

                boost::asio::async_read(
                    c->socket(),
                    boost::asio::buffer( listenerMsg->package(), message::header_length ),
                    boost::bind( &lockstepserverImpl::master_handle_reply_to_init_header,
                                 this,
                                 listenerMsg,
                                 index,
                                 boost::asio::placeholders::error ) );

                if( log_ )
                {
                    *log_ << "Master is naming socket at " << socket_address( c->socket() ) << " to '"
                          << c->name() << "'." << endl;
                }

                stringstream msgstream;
                msgstream << MsgCommands::ToClient::Init << expected_participants_count_ << ';'
                          << c->name() << ';';

                const message msg = bytebuffer( msgstream.str() );

                boost::asio::write( c->socket(),
                                    boost::asio::buffer( msg.package(), msg.package_length() ) );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_init_header(
            const shared_ptr<message>& listenerMsg,
            const size_t index,
            const boost::system::error_code& error )
        {
            if( log_ )
            {
                *log_ << "A reply was received back from " << MsgCommands::ToClient::Init
                      << " receiver." << endl;
            }

            if( ! error )
            {
                if( listenerMsg->decode_header() )
                {
                    boost::asio::async_read(
                        connections_.at( index )->socket(),
                        boost::asio::buffer( listenerMsg->contents(), listenerMsg->contents_length() ),
                        boost::bind( &lockstepserverImpl::master_handle_reply_to_init_body,
                                     this,
                                     listenerMsg,
                                     index,
                                     boost::asio::placeholders::error ) );
                }
                else
                {
                    signal_error( UnableToDecodePacketHeader );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_init_body(
            const shared_ptr<message>& listenerMsg,
            const size_t index,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                const string reply( listenerMsg->contents() );

                if( log_ )
                {
                    *log_ << "Msg received : " << reply.c_str() << endl;
                }

                bool handled = false;

                if( reply.length() >= MsgCommands::Length )
                {
                    const string cmd = reply.substr( 0, MsgCommands::Length );

                    if( cmd == MsgCommands::ToHost::Innited )
                    {
                        if( log_ )
                        {
                            *log_ << "Connection " << index << " successfully initialised."
                                  << endl;
                        }

                        master_init_connection( index + 1 );
                        handled = true;
                    }
                }
                else
                {
                    signal_error( UnexpectedMessageLength );
                }

                if( false == handled )
                {
                    signal_error( UnexpectedMessage );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_make_connections_and_begin_game()
        {
            if( expected_participants_count_ > 2 )
            {
                master_make_connections();
            }
            else
            {
                master_begin_game();
            }
        }

        void lockstepserverImpl::master_make_connections()
        {
            if( log_ )
            {
                *log_ << "Starting connection making process." << endl;
            }

            assert( expected_participants_count_ > 0 );
            assert( connections_count() == expected_participants_count_ - 1 );
            assert( connections_count() > 1 );

            master_connect_to_other_clients_or_begin_game( 0 );
        }

        void lockstepserverImpl::master_connect_to_other_clients_or_begin_game( const size_t index )
        {
            assert( index <= connections_count() );

            const size_t n_unconnected = connections_count() - ( index + 1 );

            if( n_unconnected > 0 )
            {
                if( log_ )
                {
                    *log_ << "Beginning connection process for " << index << ' '
                          << socket_address( connections_.at( index )->socket() ) << endl;
                }

                connection_sptr_array_sptr others( new connection_sptr_array() );
                others->reserve( n_unconnected );

                for( size_t i = index + 1; i < connections_.size(); ++i )
                {
                    others->push_back( connections_.at( i ) );
                }

                assert( false == others->empty() );

                master_connect_client_to_indexed_client( index, 0, others );
            }
            else
            {
                master_begin_game();
            }
        }

        void lockstepserverImpl::master_connect_client_to_indexed_client(
            const size_t index,
            const size_t otherIndex,
            const connection_sptr_array_sptr& others )
        {
            assert( index < connections_.size() );
            assert( otherIndex <= others->size() );

            if( otherIndex == others->size() )
            {
                master_connect_to_other_clients_or_begin_game( index + 1 );
            }
            else
            {
                shared_ptr<connection> other = others->at( otherIndex );

                shared_ptr<message> listenerMsg( new message() );

                boost::asio::async_read(
                    other->socket(),
                    boost::asio::buffer( listenerMsg->package(), message::header_length ),
                    boost::bind( &lockstepserverImpl::master_handle_reply_to_open_port_header,
                                 this,
                                 listenerMsg,
                                 index,
                                 otherIndex,
                                 others,
                                 boost::asio::placeholders::error ) );

                if( log_ )
                {
                    *log_ << " Master is requesting socket at " << socket_address( other->socket() )
                          << " to " << MsgCommands::ToClient::OpenPort << " for "
                          << socket_address( connections_.at( index )->socket() ) << endl;
                }

                stringstream msgstream;
                msgstream << MsgCommands::ToClient::OpenPort << connections_.at( index )->name();
                const message msg = bytebuffer( msgstream.str() );

                boost::asio::write( other->socket(),
                                    boost::asio::buffer( msg.package(), msg.package_length() ) );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_connect_header(
            const shared_ptr<message>& listenerMsg,
            const size_t index,
            const size_t otherIndex,
            const connection_sptr_array_sptr& others,
            const boost::system::error_code& error )
        {
            if( log_ )
            {
                *log_ << "A reply was received back from " << MsgCommands::ToClient::Connect
                      << " receiver." << endl;
            }

            if( ! error )
            {
                if( listenerMsg->decode_header() )
                {
                    boost::asio::async_read(
                        connections_.at( index )->socket(),
                        boost::asio::buffer( listenerMsg->contents(), listenerMsg->contents_length() ),
                        boost::bind( &lockstepserverImpl::master_handle_reply_to_connect_body,
                                     this,
                                     listenerMsg,
                                     index,
                                     otherIndex,
                                     others,
                                     boost::asio::placeholders::error ) );
                }
                else
                {
                    signal_error( UnableToDecodePacketHeader );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_connect_body(
            const shared_ptr<message>& listenerMsg,
            const size_t index,
            const size_t otherIndex,
            const connection_sptr_array_sptr& others,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                const string reply( listenerMsg->contents() );

                if( log_ )
                {
                    *log_ << "Msg received : " << reply << endl;
                }

                bool handled = false;

                if( reply.length() >= MsgCommands::Length )
                {
                    const string cmd = reply.substr( 0, MsgCommands::Length );

                    if( cmd == MsgCommands::ToHost::Connected )
                    {
                        master_connect_client_to_indexed_client( index, otherIndex + 1, others );
                        handled = true;
                    }
                }

                if( false == handled )
                {
                    signal_error( UnexpectedMessage );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_open_port_header(
            const shared_ptr<message>& listenerMsg,
            const size_t index,
            const size_t otherIndex,
            const connection_sptr_array_sptr& others,
            const boost::system::error_code& error )
        {
            if( log_ )
            {
                *log_ << "A reply was received back from " << MsgCommands::ToClient::OpenPort
                      << " receiver." << endl;
            }

            if( ! error )
            {
                if( listenerMsg->decode_header() )
                {
                    boost::asio::async_read(
                        others->at( otherIndex )->socket(),
                        boost::asio::buffer( listenerMsg->contents(), listenerMsg->contents_length() ),
                        boost::bind( &lockstepserverImpl::master_handle_reply_to_open_port_body,
                                     this,
                                     listenerMsg,
                                     index,
                                     otherIndex,
                                     others,
                                     boost::asio::placeholders::error ) );
                }
                else
                {
                    signal_error( UnableToDecodePacketHeader );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_handle_reply_to_open_port_body(
            const shared_ptr<message>& replyMsg,
            const size_t index,
            const size_t otherIndex,
            const connection_sptr_array_sptr& others,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                const string reply( replyMsg->contents() );

                if( log_ )
                {
                    *log_ << "Msg received : " << reply << endl;
                }

                bool handled = false;

                if( reply.length() >= MsgCommands::Length )
                {
                    const string cmd = reply.substr( 0, MsgCommands::Length );

                    if( cmd == MsgCommands::ToHost::PortOpen )
                    {
                        const string packet = reply.substr( MsgCommands::Length );
                        unsigned short port = get_port_from_str( packet );

                        // Tell index that otherIndex is waiting for connection from
                        // it.
                        shared_ptr<connection> c = connections_.at( index );
                        shared_ptr<connection> other = others->at( otherIndex );

                        if( log_ )
                        {
                            *log_ << socket_address( other->socket() ) << " replied with open socket at "
                                  << packet << ". Telling " << socket_address( c->socket() )
                                  << " to connect." << endl;
                        }

                        shared_ptr<message> listenerMsg( new message() );

                        boost::asio::async_read(
                            c->socket(),
                            boost::asio::buffer( listenerMsg->package(), message::header_length ),
                            boost::bind( &lockstepserverImpl::master_handle_reply_to_connect_header,
                                         this,
                                         listenerMsg,
                                         index,
                                         otherIndex,
                                         others,
                                         boost::asio::placeholders::error ) );

                        stringstream msgstream;

                        msgstream << MsgCommands::ToClient::Connect << other->name()
                                  << MsgCommands::NameSeperator
                                  << other->socket().remote_endpoint().address().to_string()
                                  << MsgCommands::PortSeperator << port;

                        const message msg = bytebuffer( msgstream.str() );
                        boost::asio::write( c->socket(),
                                            boost::asio::buffer( msg.package(), msg.package_length() ) );

                        handled = true;
                    }
                }

                if( false == handled )
                {
                    signal_error( UnexpectedMessage );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::master_begin_game()
        {
            if( log_ )
            {
                *log_ << "Beginning game." << endl;
            }

            log_socket_connectivity();

            assert( ready_to_run_loop_ == false );
            assert( closing_all_sockets_ == false );

            for( size_t i = 0; i < connections_.size(); ++i )
            {
                if( log_ )
                {
                    *log_ << "Server is sending " << MsgCommands::ToClient::Begin << " to " << i << " "
                          << socket_address( connections_[i]->socket() ) << endl;
                }

                const message msg = bytebuffer( MsgCommands::ToClient::Begin );
                boost::asio::write( connections_[i]->socket(),
                                    boost::asio::buffer( msg.package(), msg.package_length() ) );
            }

            ready_to_run_loop_ = true;
        }

        // ----------------- Client only code -----------------

        //! Constructor for client server object.
        //! This object connects to the master and then waits to be informed of
        //! other client addresses and to be told when to connect to them.
        lockstepserverImpl::lockstepserverImpl(
            const shared_ptr<lockstepserver::inetworkgameloop>& pgame,
            const string& ipaddress,
            const unsigned short port,
            const string& logfilename,
            const unsigned long tickTimeLengthMs,
            const unsigned short timeout_limit_s )
            : io_service_( new boost::asio::io_service() )
            , nop_local_message_()
            , expected_participants_count_( 0 )
            , log_( NULL )
            , timeout_limit_ms_( static_cast<long>( timeout_limit_s * 1000 ) )
            , duration_of_gametick_ms_( tickTimeLengthMs )
            , master_port_( port )
            , ready_to_run_loop_( false )
            , error_free_( true )
            , connection_ok_( true )
            , closing_all_sockets_( false )
        {
            open_log( logfilename );

            validate( pgame );

            try
            {
                client_build_function_map();

                if( log_ )
                {
                    *log_ << "Constructing client for to connect to " << ipaddress << ":" << port
                          << endl;
                }

                client_connect_to_server( ipaddress, get_str_port( master_port_ ) );

                run_service( pgame );
            }
            catch( const boost::system::system_error& e )
            {
                if( log_ )
                {
                    *log_ << "(boost::system::system_error) " << e.what() << endl;
                }

                throw lockstepserver::exception( e.what() );
            }
            catch( const lockstepserver::exception& e )
            {
                if( log_ )
                {
                    *log_ << e.what() << endl;
                }
                throw;
            }
            catch( const exception& e )
            {
                if( log_ )
                {
                    *log_ << e.what() << endl;
                }
                throw lockstepserver::exception( e.what() );
            }
        }

        void lockstepserverImpl::client_connect_to_server(
            const string& ipaddress,
            const string& port )
        {
            shared_ptr<connection> server = connection::make_shared( io_service_ );

            server->socket().open( boost::asio::ip::tcp::v4() );

            unsigned short localPort = master_port_;
            boost::system::error_code ec = find_socket_port( server, localPort );

            if( ec )
            {
                signal_error( UnableToOpenPort );
            }
            else
            {
                if( log_ )
                {
                    *log_ << "Client master connection bound to port at "
                          << server->socket().local_endpoint() << endl;
                }

                boost::asio::ip::tcp::resolver r( *io_service_ );
                boost::asio::ip::tcp::resolver::query serveraddress( ipaddress, port );
                boost::asio::ip::tcp::resolver::iterator endpoint_iterator = r.resolve( serveraddress );
                boost::asio::ip::tcp::endpoint firstendpoint = *endpoint_iterator;

                server->socket().async_connect(
                    firstendpoint,
                    boost::bind( &lockstepserverImpl::client_make_connection_to_server,
                                 this,
                                 server,
                                 boost::asio::placeholders::error,
                                 ++endpoint_iterator ) );
            }
        }

        void lockstepserverImpl::client_make_connection_to_server(
            const shared_ptr<connection>& server,
            const boost::system::error_code& error,
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
        {
            if( ! error )
            {
                if( log_ )
                {
                    *log_ << "Client master connection opened." << endl;
                }

                assert( connections_.empty() );
                add_connection( server );
                client_wait_for_messages();
            }
            else
            {
                if( endpoint_iterator != boost::asio::ip::tcp::resolver::iterator() )
                {
                    boost::asio::ip::tcp::endpoint nextendpoint = *endpoint_iterator;
                    server->socket().close();
                    server->socket().async_connect(
                        nextendpoint,
                        boost::bind( &lockstepserverImpl::client_make_connection_to_server,
                                     this,
                                     server,
                                     boost::asio::placeholders::error,
                                     ++endpoint_iterator ) );
                }
                else
                {
                    signal_error( error );
                }
            }
        }

        void lockstepserverImpl::client_wait_for_messages()
        {
            assert( connections_.size() >= 1 );

            // Listen for messages from server:
            if( log_ )
            {
                *log_ << "\nListening for messages from server..." << endl;
            }

            shared_ptr<message> messagepacket( new message() );
            boost::asio::async_read(
                connections_.at( 0 )->socket(),
                boost::asio::buffer( messagepacket->package(), message::header_length ),
                boost::bind( &lockstepserverImpl::client_handle_read_master_message_header,
                             this,
                             messagepacket,
                             boost::asio::placeholders::error ) );
        }

        void lockstepserverImpl::client_build_function_map()
        {
            functions_.clear();
            functions_.insert( command_function_map::value_type(
                                   MsgCommands::ToClient::Connect, &lockstepserverImpl::client_cmd_connect ) );
            functions_.insert( command_function_map::value_type(
                                   MsgCommands::ToClient::OpenPort, &lockstepserverImpl::client_cmd_openport ) );
            functions_.insert( command_function_map::value_type(
                                   MsgCommands::ToClient::Begin, &lockstepserverImpl::client_cmd_begin ) );
            functions_.insert( command_function_map::value_type(
                                   MsgCommands::ToClient::Init, &lockstepserverImpl::client_cmd_init ) );
        }

        // TODO Why is this string ignored?
        void lockstepserverImpl::client_cmd_begin( const string& )
        {
            if( log_ )
            {
                *log_ << "Client '" << name_ << "' ready to run." << endl;
            }

            assert( false == name_.empty() );
            assert( false == ready_to_run_loop_ );

            log_socket_connectivity();

            ready_to_run_loop_ = true;
        }

        void lockstepserverImpl::client_cmd_init( const string& packet )
        {
            if( name_.empty() )
            {
                assert( 0 == expected_participants_count_ );

                const string name = client_extract_name( packet );
                const size_t n_participants = client_extract_participants_count( packet );

                if( log_ )
                {
                    *log_ << "Client expecting " << n_participants << " participants." << endl;
                }

                expected_participants_count_ = n_participants;

                connections_.reserve( expected_participants_count_ );

                // We rename the host connection here:
                connections_.at( 0 )->rename( generate_connection_name( expected_participants_count_ ) );

                if( log_ )
                {
                    *log_ << "Master connection renamed to \"" << connections_.at( 0 )->name() << "\""
                          << endl;
                }

                name_ = name;

                if( log_ )
                {
                    *log_ << "Client renamed to \"" << name << "\"" << endl;
                }

                const message msg = bytebuffer( MsgCommands::ToHost::Innited );
                boost::asio::write( connections_.at( 0 )->socket(),
                                    boost::asio::buffer( msg.package(), msg.package_length() ) );

                client_wait_for_messages();
            }
            else
            {
                signal_error( NameAlreadySet );
            }
        }

        void lockstepserverImpl::client_cmd_connect( const string& msg )
        {
            try
            {
                shared_ptr<connection> c = connection::make_shared( io_service_ );
                c->socket().open( boost::asio::ip::tcp::v4() );

                unsigned short localPort = master_port_;
                boost::system::error_code ec = find_socket_port( c, localPort );

                if( ec )
                {
                    signal_error( UnableToOpenPort );
                }
                else
                {
                    if( log_ )
                    {
                        *log_ << "Client new connection to be opened for " << *c
                              << ".\nDecoding message '" << msg << "'\n";
                    }

                    const string::size_type n = msg.find( MsgCommands::NameSeperator );
                    const string othername = msg.substr( 0, n );
                    const string address   = msg.substr( n + 1 );

                    if( log_ )
                    {
                        *log_ << "othername = " << othername << endl;
                    }

                    const string::size_type t = address.find( MsgCommands::PortSeperator );
                    const string ip = address.substr( 0, t );
                    const string port = address.substr( t + 1 );

                    if( t == string::npos || n == string::npos || othername.empty()
                            || address.empty() || ip.empty() || port.empty() )
                    {
                        signal_error( UnknownMessage );
                    }
                    else
                    {
                        boost::asio::ip::tcp::resolver r( *io_service_ );
                        boost::asio::ip::tcp::resolver::query serveraddress( ip, port );
                        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = r.resolve( serveraddress );
                        boost::asio::ip::tcp::endpoint firstendpoint = *endpoint_iterator;

                        c->rename( othername );

                        c->socket().async_connect(
                            firstendpoint,
                            boost::bind( &lockstepserverImpl::client_make_connection_to_client,
                                         this,
                                         c,
                                         boost::asio::placeholders::error,
                                         ++endpoint_iterator ) );
                    }
                }
            }
            catch( const exception& e )
            {
                if( log_ )
                {
                    *log_ << "Exception " << e.what() << endl;
                }

                signal_error( UnknownMessage );
            }
            catch( ... )
            {
                signal_error( UnknownMessage );
            }
        }

        void lockstepserverImpl::client_cmd_openport( const string& name )
        {
            shared_ptr<connection> cnct = connection::make_shared( io_service_ );
            cnct->rename( name );

            const unsigned int nextport = ( static_cast<unsigned int>( local_port() ) + 1u ) % USHRT_MAX;
            shared_ptr<tcpacceptor> accptr = create_acceptor( static_cast<unsigned short>( nextport ) );

            if( accptr )
            {
                // This exists purely for convenience of logging. It allows us to capture
                // the endpoint of the acceptor, as the sockets seem to take a slightly
                // more ambiguous view of which end is the 'end point' which can make the
                // logs confusing.
                shared_ptr<tcpendpoint> acceptorendpoint = make_shared<boost::asio::ip::tcp::endpoint>();

                accptr->async_accept( cnct->socket(),
                                      *acceptorendpoint,
                                      boost::bind( &lockstepserverImpl::client_accept_new_client,
                                                   this,
                                                   cnct,
                                                   accptr,
                                                   acceptorendpoint,
                                                   boost::asio::placeholders::error ) );

                stringstream msgstream;

                boost::system::error_code ec;
                auto endpnt = accptr->local_endpoint();

                if( ec )
                {
                    signal_error( MissingEndpoint );
                }
                else
                {
                    msgstream << MsgCommands::ToHost::PortOpen << endpnt.port();

                    const message msg = bytebuffer( msgstream.str() );

                    boost::asio::write( connections_.at( 0 )->socket(),
                                        boost::asio::buffer( msg.package(), msg.package_length() ) );

                    client_wait_for_messages();
                }
            }
        }

        string lockstepserverImpl::client_extract_name( const string& packet )
        {
            string name;

            const string::size_type p1 = packet.find_first_of( ';' );
            string::size_type p2 = string::npos;

            if( string::npos == p1 )
            {
                signal_error( InvalidCmdPacket );
            }
            else
            {
                p2 = packet.find_first_of( ';', p1 + 1 );

                if( string::npos == p2 )
                {
                    signal_error( InvalidCmdPacket );
                }

                name = packet.substr( p1 + 1, p2 - p1 - 1 );

                if( false == is_valid_client_name( name ) )
                {
                    signal_error( InvalidCmdPacket );
                }
            }

            assert( false == name.empty() );

            return name;
        }

        size_t lockstepserverImpl::client_extract_participants_count( const string& packet )
        {
            size_t participants = 0;

            try
            {
                stringstream ss;
                ss.exceptions( ios::failbit );
                ss << packet;
                ss >> participants;
            }
            catch( ... )
            {
                signal_error( InvalidCmdPacket );
            }

            if( participants < 2 )
            {
                signal_error( InvalidCmdPacket );
            }

            return participants;
        }

        void lockstepserverImpl::client_make_connection_to_client(
            const shared_ptr<connection>& otherClient,
            const boost::system::error_code& error,
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
        {
            if( ! error )
            {
                if( log_ )
                {
                    *log_ << "Client-to-client connection opened, connection " << *otherClient
                          << endl;
                }

                assert( false == connections_.empty() );
                add_connection( otherClient );

                stringstream msgstream;
                msgstream << MsgCommands::ToHost::Connected << socket_address( otherClient->socket() );
                const message msg = bytebuffer( msgstream.str() );
                boost::asio::write( connections_.at( 0 )->socket(),
                                    boost::asio::buffer( msg.package(), msg.package_length() ) );

                client_wait_for_messages();
            }
            else
            {
                if( endpoint_iterator != boost::asio::ip::tcp::resolver::iterator() )
                {
                    boost::asio::ip::tcp::endpoint nextendpoint = *endpoint_iterator;
                    otherClient->socket().close();
                    otherClient->socket().async_connect(
                        nextendpoint,
                        boost::bind( &lockstepserverImpl::client_make_connection_to_client,
                                     this,
                                     otherClient,
                                     boost::asio::placeholders::error,
                                     ++endpoint_iterator ) );
                }
                else
                {
                    signal_error( error );
                }
            }
        }

        void lockstepserverImpl::client_accept_new_client(
            const shared_ptr<connection>& client,
            const shared_ptr<tcpacceptor>&,
            const shared_ptr<tcpendpoint>& acceptorendpoint,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                if( log_ )
                {
                    *log_ << "Client accepted new client connection; endpoint "
                          "after accept is " << acceptorendpoint->address()
                          << ":" << acceptorendpoint->port()
                          << ", connection is " << *client << endl;
                }

                if( players_count() < expected_participants_count_ )
                {
                    add_connection( client );
                }
                else if( players_count() >= expected_participants_count_ )
                {
                    signal_error( LogicError );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::client_handle_read_master_message_header(
            const shared_ptr<message>& messagepacket,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                if( messagepacket->decode_header() )
                {
                    boost::asio::async_read(
                        connections_.at( 0 )->socket(),
                        boost::asio::buffer( messagepacket->contents(),
                                             messagepacket->contents_length() ),
                        boost::bind( &lockstepserverImpl::client_handle_read_master_message_body,
                                     this,
                                     messagepacket,
                                     boost::asio::placeholders::error ) );
                }
                else
                {
                    signal_error( UnableToDecodePacketHeader );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::client_handle_read_master_message_body(
            const shared_ptr<message>& messagepacket,
            const boost::system::error_code& error )
        {
            assert( false == ready_to_run_loop_ );

            if( ! error )
            {
                // Search the string range for the nul terminator. Use this as the
                // length of the packet,
                // or else use the packet length.
                const byte* nul = reinterpret_cast<const byte*>(
                                      memchr( messagepacket->contents(), 0, messagepacket->contents_length() ) );
                size_t len = nul ? nul - messagepacket->contents() : messagepacket->contents_length();
                assert( len <= messagepacket->contents_length() );
                string messagetext( messagepacket->contents(), len );

                if( log_ )
                {
                    *log_ << "\nMessage received was " << messagetext << endl;
                }

                client_process_message( messagetext );
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::client_process_message( const string& msg )
        {
            bool handled = false;

            if( msg.length() >= MsgCommands::Length )
            {
                const string cmd    = msg.substr( 0, MsgCommands::Length );
                const string packet = msg.substr( MsgCommands::Length, msg.size() - MsgCommands::Length );

                command_function_map::const_iterator i = functions_.find( cmd );

                if( i != functions_.end() )
                {
                    if( log_ )
                    {
                        *log_ << "Invoking message received \"" << cmd << "\" with " << packet
                              << endl;
                    }

                    ( this->*( i->second ) )( packet );
                    handled = true;
                }
            }

            if( false == handled )
            {
                if( log_ )
                {
                    *log_ << "Unknown message received \"" << msg << "\"" << endl;
                }

                signal_error( UnknownMessage );
            }
        }

        // ------------ Shared master and client code ------------

        lockstepserverImpl::~lockstepserverImpl()
        {
            io_service_->stop();
            close_all_sockets();

            log_ = 0;

            if( logfile_.is_open() )
            {
                logfile_.close();
            }
        }

        void lockstepserverImpl::log_socket_connectivity() const
        {
            if( log_ )
            {
                for( size_t i = 0; i < connections_.size(); ++i )
                {
                    *log_ << *connections_[i] << '\n';
                }

                if( connections_.empty() )
                {
                    *log_ << "No sockets connected.\n";
                }

                *log_ << endl;
            }
        }

        void lockstepserverImpl::close_all_sockets()
        {
            if( false == connections_.empty() )
            {
                log_socket_connectivity();

                connections_.clear();

                if( log_ )
                {
                    *log_ << "Closed sockets.\n";
                }
            }
        }

        //! Sets an error flag which is picked up by main thread.
        //! Main thread should handle all error cases.
        void lockstepserverImpl::signal_error( const boost::system::error_code& error )
        {
            boost::lock_guard<boost::mutex> lock( mut_ );

            if( false == closing_all_sockets_ )
            {
                if( false == is_expected_nothrow_socket_closing_error( error ) )
                {
                    if( log_ )
                    {
                        stringstream ss;
                        ss << "Encountered error \"" << error.message()
                           << "\", category = \""
                           << error.category().name()
                           << "\"; with " << connections_.size()
                           << " connections.";
                        const string msg = ss.str();
                        *log_ << msg << endl;
                    }

                    if( error_free_ )
                    {
                        error_ = error;
                        error_free_ = false;
                    }
                }

                connection_ok_ = false;
            }
        }

        bool lockstepserverImpl::is_expected_nothrow_socket_closing_error(
            const boost::system::error_code& e ) const
        {
            bool is_expected = false;

            // TODO - examine errors here and sift for closing ones.

            switch( e.value() )
            {
                case 10054:
                    // "An existing connection was forcibly closed by the remote host"
                    is_expected = true;
                    break;

                default:
                    break;
            }

            return is_expected;
        }

        void lockstepserverImpl::signal_error( LockstepServerErrorType e )
        {
            static const lockstepserver_error_category error_catagory;
            signal_error( boost::system::error_code( e, error_catagory ) );
        }

        void lockstepserverImpl::validate( const shared_ptr<lockstepserver::inetworkgameloop>& pgame ) const
        {
            if( nullptr == pgame.get() )
            {
                throw lockstepserver::exception( "lockstepserver::inetworkgameloop passed in was null." );
            }
        }

        void lockstepserverImpl::validate( const size_t /*expected_participants_count*/ ) const
        {
            //if (expected_participants_count < 1)
            //{
            //    throw lockstepserver::exception("Not enough participants. At least 1 is needed.");
            //}
        }

        void lockstepserverImpl::loop_get_frame_update_messages_from_clients()
        {
            for_each( connections_.begin(),
                      connections_.end(),
                      boost::bind( &lockstepserverImpl::loop_post_listen_for_message, this, _1 ) );
        }

        void lockstepserverImpl::loop_post_listen_for_message( const shared_ptr<connection>& c )
        {
            auto msg = make_shared<message>();
            boost::asio::async_read( c->socket(),
                                     boost::asio::buffer( msg->package(), message::header_length ),
                                     boost::bind( &lockstepserverImpl::loop_game_message_read_header,
                                                  this,
                                                  c,
                                                  msg,
                                                  boost::asio::placeholders::error ) );
        }

        void lockstepserverImpl::loop_game_message_read_header(
            const shared_ptr<connection>& c,
            const shared_ptr<message>& messagepacket,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                if( messagepacket->decode_header() )
                {
                    if( messagepacket->contents_length() > 0 )
                    {
                        boost::asio::async_read(
                            c->socket(),
                            boost::asio::buffer( messagepacket->contents(),
                                                 messagepacket->contents_length() ),
                            boost::bind( &lockstepserverImpl::loop_game_message_read_body,
                                         this,
                                         c,
                                         messagepacket,
                                         boost::asio::placeholders::error ) );
                    }
                    else
                    {
                        boost::lock_guard<boost::mutex> lock( mut_ );
                        assert( client_messages_.find( c->name() ) == client_messages_.end() );
                        client_messages_[c->name()] = bytebuffer();

                        if( log_ )
                        {
                            *log_ << "Received empty message #" << client_messages_.size() - 1
                                  << " from " << socket_address( c->socket() ) << endl;
                        }
                    }
                }
                else
                {
                    signal_error( UnableToDecodePacketHeader );
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::loop_game_message_read_body(
            const shared_ptr<connection>& c,
            const shared_ptr<message>& messagepacket,
            const boost::system::error_code& error )
        {
            if( ! error )
            {
                boost::lock_guard<boost::mutex> lock( mut_ );
                assert( client_messages_.find( c->name() ) == client_messages_.end() );
                client_messages_[c->name()] = bytebuffer( messagepacket->contents(), messagepacket->contents_length() );

                if( log_ )
                {
                    *log_ << "Received message #" << client_messages_.size() - 1 << '\n';

                    log_message( messagepacket->contents(), messagepacket->contents_length() );

                    *log_ << "\n from socket '" << c->name() << "' " << c->socket().local_endpoint()
                          << " -> " << c->socket().remote_endpoint() << endl;
                }
            }
            else
            {
                signal_error( error );
            }
        }

        void lockstepserverImpl::insert_message( const bytebuffer& msg )
        {
            assert( local_message_.is_empty() );
            local_message_ = msg;
        }

        void lockstepserverImpl::add_connection( const shared_ptr<connection>& c )
        {
            c->nagle( false );

            connections_.push_back( c );

            if( log_ )
            {
                *log_ << "Connection index =" << ( connections_.size() - 1 )
                      << ", name = \"" << c->name()
                      << "\" from (remote) " << c->socket().remote_endpoint()
                      << " to (local) " << c->socket().local_endpoint()
                      << " added." << endl;
            }
        }

        string lockstepserverImpl::get_str_port( unsigned short port_number ) const
        {
            stringstream ss;
            ss << port_number;
            return ss.str();
        }

        string lockstepserverImpl::generate_connection_name( size_t index ) const
        {
            stringstream ss;
            ss << index;
            assert( is_valid_client_name( ss.str() ) );
            return ss.str();
        }

        string lockstepserverImpl::this_player_name() const
        {
            assert( false == name_.empty() );
            return name_;
        }

        vector<string> lockstepserverImpl::other_player_names() const
        {
            vector<string> names;
            names.reserve( connections_.size() );

            for_each( connections_.cbegin(),
                      connections_.cend(),
                      [&]( const shared_ptr<connection>& p )
            { names.push_back( p->name() ); } );

            return names;
        }

        bool lockstepserverImpl::is_valid_client_name( const string& name ) const
        {
            return name.length() > 0;
        }

        unsigned short lockstepserverImpl::get_port_from_str( const string& portn ) const
        {
            unsigned short port = 0;
            istringstream ss;
            ss.exceptions( ios::failbit );
            ss.str( portn );
            ss >> port;
            return port;
        }

        string lockstepserverImpl::socket_address( const boost::asio::ip::tcp::socket& sckt ) const
        {
            return endpoint_address( sckt.remote_endpoint() );
        }

        string lockstepserverImpl::endpoint_address( const boost::asio::ip::tcp::endpoint& e ) const
        {
            stringstream ss;
            ss << e.address().to_string() << ":" << e.port();
            return ss.str();
        }

        //! This may return a null pointer.
        shared_ptr<lockstepserverImpl::tcpacceptor> lockstepserverImpl::create_acceptor(
            const unsigned short port )
        {
            auto a = make_shared<boost::asio::ip::tcp::acceptor>( *io_service_ );

            a->open( boost::asio::ip::tcp::v4() );
            a->set_option( boost::asio::ip::tcp::acceptor::reuse_address( false ) );

            boost::system::error_code ec;

            unsigned short testport = port;

            do
            {
                boost::asio::ip::tcp::endpoint acceptorendpoint( boost::asio::ip::tcp::v4(), testport );
                a->bind( acceptorendpoint, ec );
                ++testport;
            }
            while( ec && testport != port );

            if( ec )
            {
                if( log_ )
                {
                    *log_ << "Acceptor unable to find suitable port." << endl;
                }

                signal_error( ec );

                a.reset();
            }
            else
            {
                if( log_ )
                {
                    *log_ << "Created acceptor with local endpoint "
                          << endpoint_address( a->local_endpoint() ) << endl;
                }

                a->listen();
            }

            return a;
        }

        boost::system::error_code lockstepserverImpl::find_socket_port(
            const shared_ptr<connection>& conn,
            unsigned short& startingPort ) const
        {
            boost::system::error_code ec;

            unsigned short localPort = startingPort;

            do
            {
                ++localPort;
                conn->socket().bind(
                    boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), localPort ), ec );
            }
            while( ec && ( localPort != startingPort ) );

            if( ! ec )
            {
                startingPort = localPort;
            }

            return ec;
        }

        unsigned short lockstepserverImpl::local_port() const
        {
            return connections_.at( 0 )->socket().local_endpoint().port();
        }

        bool lockstepserverImpl::waiting_for_more_messages() const
        {
            boost::lock_guard<boost::mutex> lock( mut_ );
            return ( client_messages_.size() < connections_.size() );
        }

        // TODO: Remove conditional compilations.
#ifdef SS_NO_LOCKSTEP_LOG
        void lockstepserverImpl::open_log( const string& )
        {
        }
#else
        void lockstepserverImpl::open_log( const string& logfilename )
        {
            if( logfile_.is_open() )
            {
                logfile_.flush();
                logfile_.close();
            }

            log_ = nullptr;

            if( logfilename.empty() )
            {
                // log_ = solosnake::g_clog;
            }
            else
            {
                logfile_.open( logfilename.c_str() );

                if( false == logfile_.is_open() )
                {
                    const string message( string( "Unable to open logfile " ) + logfilename );
                    ss_err( message );
                    throw lockstepserver::exception( message.c_str() );
                }
                else
                {
                    log_ = &logfile_;
                }
            }
        }
#endif

        //! Writes content to open log stream.
        void lockstepserverImpl::log_message( const byte* data, const size_t linelength ) const
        {
            assert( log_ );

            // Print contents as hex when possible.
            for( size_t i = 0; i < linelength; i += 16 )
            {
                const size_t rowend = min( i + 16, linelength );

                for( size_t j = i; j < rowend; ++j )
                {
                    print_byte( data[j], *log_ );
                }

                if( ( i + 1 ) < linelength )
                {
                    *log_ << '\n';
                }
            }
        }

        void lockstepserverImpl::log_client_messages() const
        {
            assert( log_ );

            *log_ << "Messages for client " << name_ << " to process are:\n";

            for_each( client_messages_.cbegin(),
                      client_messages_.cend(),
                      [&]( const pair<string, bytebuffer>& m )
            {
                if( m.second.is_empty() )
                {
                    *log_ << '\t' << m.first << ", Empty\n";
                }
                else
                {
                    *log_ << '\t' << m.first << '\n';
                    log_message( m.second.data(), m.second.size() );
                    *log_ << '\n';
                }
            } );
        }

        // ------------------------ MAIN LOOP -------------------------

        void lockstepserverImpl::run_service(
            const shared_ptr<lockstepserver::inetworkgameloop>& pgame )
        {
            auto timing = make_timer();
            assert( timing );

            work_sptr pwork = make_shared<boost::asio::io_service::work>( *io_service_.get() );

            boost::thread t( boost::bind( &boost::asio::io_service::run, io_service_.get() ) );

            pgame->initial_preframe_update();

            timing->restart();

            // Set to false when the user chooses to quit during the pre-frame
            // update phase (e.g. closes app).
            bool want_main_loop = true;

            while( want_main_loop && error_free_ && connection_ok_ && ( false == ready_to_run_loop_ ) )
            {
                const unsigned long elapsed_ms = timing->elapsed_milliseconds();

                want_main_loop = LoopAgain == pgame->preframe_update( elapsed_ms );

                if( timeout_limit_ms_ > 0 && elapsed_ms > timeout_limit_ms_ )
                {
                    signal_error( ConnectionTimeout );
                }

                timing->restart();
            }

            if( error_free_ )
            {
                pgame->setup_participants( this_player_name(), other_player_names() );
            }

            bool run_loop = want_main_loop && LoopAgain == pgame->initial_frame_update();

            unsigned long frame_counter = 1;

            timing->restart();

            while( run_loop && error_free_ && connection_ok_ )
            {
                // Begin a Frame
                timing->restart();

                if( log_ )
                {
                    *log_ << "\nFrame " << frame_counter << '\n';
                }

                client_messages_.clear();

                io_service_->post( boost::bind(
                                       &lockstepserverImpl::loop_get_frame_update_messages_from_clients, this ) );

                local_message_.clear();

                pgame->get_frame_message( local_message_ );

                const bytebuffer& message_to_send =
                    local_message_.is_empty() ? nop_local_message_ : local_message_;

                for_each( connections_.cbegin(),
                          connections_.cend(),
                          [&]( shared_ptr<connection> c )
                {
                    if( error_free_ )
                    {
                        loop_post_message_to_connection( c, message_to_send );
                    }
                } );

                const unsigned long timeAtStartOfUpdateLoop = timing->elapsed_milliseconds();
                unsigned long timeSinceLoopBegan = 0;
                unsigned long timeOfLastLoop = timeAtStartOfUpdateLoop;

                do
                {
                    const unsigned long timeNow = timing->elapsed_milliseconds();
                    timeSinceLoopBegan = timeNow - timeAtStartOfUpdateLoop;
                    if( timeNow > timeOfLastLoop )
                    {
                        run_loop = LoopAgain == pgame->frame_update( timeNow - timeOfLastLoop );
                        timeOfLastLoop = timeNow;
                    }
                }
                while(
                    error_free_ && run_loop && connection_ok_
                    && ( waiting_for_more_messages() || timeSinceLoopBegan < duration_of_gametick_ms_ ) );

                if( run_loop && error_free_ )
                {
                    boost::lock_guard<boost::mutex> lock( mut_ );
                    assert( client_messages_.find( name_ ) == client_messages_.end() );
                    client_messages_[name_] = local_message_;

                    if( log_ )
                    {
                        log_client_messages();
                    }

                    ++frame_counter;

                    // if( 0 == (frame_counter%10))
                    //{
                    //    ss_log( "10 frames ", frame_counter );
                    //}

                    pgame->end_frame( client_messages_, timeSinceLoopBegan );
                }

                timing->restart();
            };

            if( want_main_loop )
            {
                pgame->final_frame_update( timing->elapsed_milliseconds() );
            }

            if( log_ )
            {
                *log_ << "Run loop ending..." << endl;
            }

            closing_all_sockets_ = true;

            io_service_->dispatch( boost::bind( &lockstepserverImpl::close_all_sockets, this ) );

            io_service_->stop();

            t.join();

            pwork.reset();

            if( false == error_free_ )
            {
                throw lockstepserver::exception( error_.message().c_str() );
            }

            if( log_ )
            {
                *log_ << "Run loop ended." << endl;
            }
        }

        // ---------------------- lockstepserver ----------------------

        lockstepserver::inetworkgameloop::~inetworkgameloop()
        {
            // NOP
        }

        //! Host
        lockstepserver::lockstepserver( const shared_ptr<inetworkgameloop>& game,
                                        size_t expected_participants_count,
                                        unsigned short port,
                                        unsigned long tickTimeLengthMs,
                                        unsigned short timeout_s,
                                        string logfilename )
            : pimpl_(
                new lockstepserverImpl( game,
                                        expected_participants_count,
                                        port,
                                        logfilename,
                                        tickTimeLengthMs,
                                        timeout_s ) )
        {
            // NOP
        }

        //! Client
        lockstepserver::lockstepserver( const shared_ptr<inetworkgameloop>& game,
                                        const string& hostipaddress,
                                        unsigned short port,
                                        unsigned long tickTimeLengthMs,
                                        unsigned short timeout_s,
                                        string logfilename )
            : pimpl_(
                new lockstepserverImpl( game,
                                        hostipaddress,
                                        port,
                                        logfilename,
                                        tickTimeLengthMs,
                                        timeout_s ) )
        {
            // NOP
        }

        lockstepserver::~lockstepserver()
        {
            pimpl_.reset();
        }

        void lockstepserver::insert_message( const bytebuffer& m )
        {
            pimpl_->insert_message( m );
        }
    }
}
