#define RUN_TESTS

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include "solosnake/testing/testing.hpp"
#include "solosnake/bytebuffer.hpp"
#include "solosnake/lockstepserver.hpp"
#include "solosnake/loopresult.hpp"

using namespace std::literals;
using namespace solosnake::network;

namespace
{
    class thread_group
    {
    public:

        void add_thread( std::unique_ptr<std::thread> );

        void join_all();

    private:
        std::vector<std::unique_ptr<std::thread>> threads_;
    };

    void thread_group::add_thread( std::unique_ptr<std::thread> t )
    {
        threads_.push_back( std::move(t) );
    }

    void thread_group::join_all()
    {
        for( auto& t : threads_ ) 
        {
            t->join();
        }
    }

    unsigned short get_fresh_port_number()
    {
        static unsigned short port = 2000;
        port += 10u;
        return port;
    }

    const size_t ExpectedParticipantsCount = 3;

    typedef std::vector<std::vector<bytebuffer>> bytebuffer_array_array;
    typedef std::shared_ptr<bytebuffer_array_array> bytebuffer_array_array_ptr;

    class testigame : public solosnake::network::lockstepserver::inetworkgameloop
    {
    public:
        testigame( size_t max_frames, bytebuffer_array_array_ptr messages )
            : connecting_update_counter_( 0 )
            , frame_counter_( 0 )
            , max_frames_( max_frames )
            , saved_messages_( messages )
        {
            ::srand( static_cast<unsigned int>( ::time( 0 ) ) );
            saved_messages_->clear();
            saved_messages_->reserve( max_frames );
        }

        virtual ~testigame()
        {
        }

        void initial_preframe_update() override
        {
        }

        solosnake::LoopResult preframe_update( const unsigned long ) override
        {
            ++connecting_update_counter_;
            return solosnake::LoopAgain;
        }

        solosnake::LoopResult initial_frame_update() override
        {
            return solosnake::LoopAgain;
        }

        void final_frame_update( const unsigned long ) override
        {
        }

        solosnake::LoopResult frame_update( const unsigned long ) override
        {
            return frame_counter_ < max_frames_ ? solosnake::LoopAgain : solosnake::StopLooping;
        }

        void get_frame_message( bytebuffer& msg ) override
        {
            if( frame_counter_ < max_frames_ )
            {
                assert( !myname_.empty() );

                std::stringstream ss;
                ss << myname_;
                std::string s = ss.str();

                msg = bytebuffer( s );
            }
        }

        void end_frame( const std::map<std::string, bytebuffer>& clientMessages,
                        const unsigned long ) override
        {
            if( frame_counter_ < max_frames_ )
            {
                std::ostringstream oss;
                oss << msgs_txt_;

                for( auto i = clientMessages.cbegin(); i != clientMessages.cend(); ++i )
                {
                    frame_messages_.push_back( i->second );
                    oss << i->second.data();
                }

                saved_messages_->push_back( frame_messages_ );
                frame_messages_.clear();

                msgs_txt_ = oss.str();

                ++frame_counter_;
            }
        }

        void setup_participants( const std::string& me,
                                 const std::vector<std::string>& ) override
        {
            myname_ = me;
            assert( !myname_.empty() );
        }

    private:
        std::string                     myname_;
        std::string                     msgs_txt_;
        std::vector<bytebuffer>         frame_messages_;
        bytebuffer_array_array_ptr      saved_messages_;
        size_t                          connecting_update_counter_;
        size_t                          frame_counter_;
        size_t                          max_frames_;
    };

    void start_host( size_t n_participants,
                     size_t max_frames,
                     unsigned short port,
                     bytebuffer_array_array_ptr messages,
                     std::string log )
    {
        auto pigame = std::make_shared<testigame>( max_frames, messages );
        auto host   = std::make_shared<solosnake::network::lockstepserver>( pigame, n_participants, port, 100, 5 * 60, log );
    }

    void start_client( size_t n_participants,
                       size_t max_frames,
                       unsigned short port,
                       bytebuffer_array_array_ptr messages,
                       std::string log )
    {
        if( n_participants > 1 )
        {
            std::this_thread::sleep_for(2000ms);
            auto pigame = std::make_shared<testigame>(max_frames, messages);
            auto client = std::make_shared<solosnake::network::lockstepserver>( pigame, "127.0.0.1", port, 100, 5 * 60, log );
        }
        else
        {
            start_host( n_participants, max_frames, port, messages, log );
        }
    }

    std::vector<bytebuffer_array_array_ptr> make_messages_array(const unsigned int n) 
    {
        std::vector<bytebuffer_array_array_ptr> messages;

        for (unsigned int i = 0; i < n; ++i)
        {
            messages.push_back(std::make_shared<bytebuffer_array_array>());
        }

        return messages;
    }

    std::string make_logfile_name( const unsigned int instances, const unsigned int i )
    {
        return std::string(std::to_string(instances) + "" + std::to_string(i) + ".txt");
    }

    std::vector<std::function<void()>> make_client_functions(const unsigned int clients, 
                                                             const unsigned int max_frames,
                                                             const unsigned short port,
                                                             const bool usefile,
                                                             const std::vector<bytebuffer_array_array_ptr>& messages)
    {
        const unsigned int instances = clients + 1;

        std::vector<std::function<void()>> functions;

        for (unsigned int i = 0; i < clients; ++i)
        {
            std::string resultfilename = usefile ? make_logfile_name(instances, i) : std::string();
            std::function<void()> fn = [=]() { start_client(instances, max_frames, port, messages.at(i), resultfilename); };
            functions.push_back(std::move(fn));
        }

        return functions;
    }

    void test_run_host_locally_and_clients_in_threads(const unsigned int instances, const bool usefile)
    {
        ASSERT_TRUE(instances > 0);

        std::this_thread::sleep_for(5000ms);

        const unsigned int max_frames = 60;
        const unsigned int clients    = instances - 1;
        const unsigned short port     = get_fresh_port_number();

        auto messages = make_messages_array(instances);
        auto mainFns  = make_client_functions(clients, max_frames, port, usefile, messages);

        thread_group group;

        for (unsigned int i = 0; i < clients; ++i)
        {
            group.add_thread(std::make_unique<std::thread>(std::move(mainFns.at(i))));
        }

        std::string resultfilename = usefile ? make_logfile_name(instances, clients) : std::string();
        start_host(instances, max_frames, port, messages.at(clients), resultfilename);

        group.join_all();

        ASSERT_TRUE(messages.at(0)->size() == max_frames);

        for (unsigned int i = 1; i < instances; ++i)
        {
            ASSERT_TRUE(*messages.at(0) == *messages.at(i));
        }

        std::this_thread::sleep_for(5000ms);
    }
}

#ifdef RUN_TESTS

TEST( lockstepserver, one_client_using_fstream )
{
    std::this_thread::sleep_for(5000ms);

    const size_t max_frames = 60;

    bytebuffer_array_array_ptr client1_messages(new bytebuffer_array_array());

    start_client(1, max_frames, get_fresh_port_number(), client1_messages, "1_client1.txt");

    EXPECT_TRUE(client1_messages->size() == max_frames);

    std::this_thread::sleep_for(5000ms);
}

TEST( lockstepserver, one_host_used_fstream )
{
    std::this_thread::sleep_for(5000ms);

    const size_t max_frames = 60;

    bytebuffer_array_array_ptr hosts_messages(new bytebuffer_array_array());

    start_host(1, max_frames, get_fresh_port_number(), hosts_messages, "1_host.txt");

    EXPECT_TRUE(hosts_messages->size() == max_frames);

    std::this_thread::sleep_for(5000ms);
}

TEST( lockstepserver, two_clients_host_used_fstream )
{
    test_run_host_locally_and_clients_in_threads(2, true);
}

TEST( lockstepserver, three_clients_host_used_fstream )
{

    test_run_host_locally_and_clients_in_threads(3, true);
}

TEST( lockstepserver, four_clients_host_used_fstream )
{
    test_run_host_locally_and_clients_in_threads(4, true);
}

TEST( lockstepserver, four_clients_no_fstream )
{
    test_run_host_locally_and_clients_in_threads(4, false);
}

TEST( lockstepserver, eight_clients_host_used_fstream )
{
    test_run_host_locally_and_clients_in_threads(8, true);
}

#endif
