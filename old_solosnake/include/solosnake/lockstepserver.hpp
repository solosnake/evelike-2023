#ifndef solosnake_lockstepserver_hpp
#define solosnake_lockstepserver_hpp

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <string>
#include <memory>
#include <iosfwd>
#include <exception>
#include "solosnake/igameloop.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    namespace network
    {
        class bytebuffer;
        class lockstepserverImpl;

        class lockstepserver
        {
        public:

            class exception : public SS_EXCEPTION_TYPE
            {
            public:
                explicit exception( const char* what );
            };

            //! All participating games of the server must inherit from this
            //! class.
            class inetworkgameloop : public igameloop
            {
            public:

                virtual ~inetworkgameloop();

                //! Called at least once, to inform the class of its own name and
                // the participants names.
                //! Guaranteed to be called prior to any frame calls.
                virtual void setup_participants(
                    const std::string& yourName,
                    const std::vector<std::string>& othersNames ) = 0;

                //! Called once per frame, it is a request from the server
                //! for this games contribution to the total frame's messages.
                //! The game should fill in the given bytebuffer object with
                //! whatever chunk of information it wishes to be mirrored to
                //! all the other game client, for this frame.
                //! The message will be rebroadcast to all other clients.
                virtual void get_frame_message(
                    bytebuffer& ) = 0;

                //! The client messages, one per client participating in the
                //! game. This will include the local clients own message for
                //! this frame. All clients receive exactly the same
                //! messages.
                virtual void end_frame(
                    const std::map<std::string, bytebuffer>&,
                    const unsigned long milliseconds ) = 0;
            };

            //! Create and start a lock-stepped server HOST object.
            lockstepserver(
                const std::shared_ptr<inetworkgameloop>&,
                size_t expected_participants_count,
                unsigned short port,
                unsigned long tickTimeLengthMs,
                unsigned short timeout_s,
                std::string logfilename = std::string() );

            //! Create and start a lock-stepped server CLIENT object.
            lockstepserver(
                const std::shared_ptr<inetworkgameloop>&,
                const std::string& hostipaddress,
                unsigned short port,
                unsigned long tickTimeLengthMs,
                unsigned short timeout_s,
                std::string logfilename = std::string() );

            virtual ~lockstepserver();

        private:

            void insert_message( const bytebuffer& );

            lockstepserver( const lockstepserver& );

            lockstepserver& operator=( const lockstepserver& );

        private:

            std::unique_ptr<lockstepserverImpl> pimpl_;
        };
    }
}

#endif
