#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#pragma warning(push, 1)
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#pragma warning(pop)
#include "solosnake/testing/testing.hpp"
#include "solosnake/network/network.hpp"
#include "solosnake/game/namemapper.hpp"
#include "solosnake/game/known.hpp"
#include "solosnake/game/actorsystem.hpp"
#include "solosnake/game/actorcommandsystem.hpp"
#include "solosnake/game/idallocator.hpp"
#include "solosnake/game/actortemplate.hpp"
#include "solosnake/game/actor.hpp"
#include "solosnake/game/state.hpp"
#include "solosnake/game/match.hpp"
#include "solosnake/game/player.hpp"
#include "solosnake/game/broadcaster.hpp"

using namespace solosnake::base;
using namespace solosnake::game;
using namespace solosnake::network;

namespace
{
const unsigned short Port = 2001;

typedef std::vector<std::vector<std::string>> string_array;
typedef std::tr1::shared_ptr<string_array> string_array_ptr;

class commandtranslator
{
public:
    command translate_to_command(const byte* msg)
    {
        const size_t* p = reinterpret_cast<const size_t*>(msg);

        return command(p[0], p[1], p[2]);
    }

    void translate_to_commands(const bytebuffer& msg, commandlist& tranlatedcommands)
    {
        tranlatedcommands.clear();

        if (!msg.is_empty())
        {
            assert(0 == (msg.size() % (sizeof(size_t) * 3)));

            const byte* p = msg.data();
            const byte* end = msg.data() + msg.size();

            while (p != end)
            {
                const command c = translate_to_command(p);

                /*if( tranlatedcommands.is_empty() )
                    {
                        tranlatedcommands.set_step( c.get_step() );
                    }*/

                tranlatedcommands.add_command(c);

                p += (sizeof(size_t) * 3);
            }
        }
    }

    bytebuffer translate_to_message(const command& c)
    {
        const size_t inputs[3] = { c.from(), c.to(), c.what() };
        bytebuffer buffer(&inputs, sizeof(inputs));
        return buffer;
    }
};

typedef std::tr1::shared_ptr<commandtranslator> commandtranslator_ptr;

class movesystem : public actorcommandsystem
{
public:
    movesystem(namemapper_ptr names) : actorcommandsystem(L"move", names)
    {
        traits_.push_back(actortrait(L"X", FloatType));
        traits_.push_back(actortrait(L"Y", FloatType));
    }

    virtual ~movesystem()
    {
    }

    virtual actorsystem_ptr clone()
    {
        return shared_from_this();
    }

    virtual const std::vector<actortrait>& traits_required() const
    {
        return traits_;
    }

    virtual const std::vector<actortrait>& traits_required_on_target() const
    {
        return traits_;
    }

    virtual bool advancing(const state&)
    {
        return true;
    }

    virtual void wakening(const command&)
    {
    }

private:
    std::vector<actortrait> traits_;
};

actortemplate_ptr create_test_system()
{
    namemapper_ptr nameIndices(new namemapper());

    // Create and add position system.
    known<actorsystem> knownsystems; // List of known systems.
                                     /*actorsystem_ptr possys( new positionsystem(nameIndices) );
knownsystems.add( possys );*/

    // Create and add move system.
    known<actorcommandsystem> knowncommands; // List of known commands.
    actorcommandsystem_ptr movecsys(new movesystem(nameIndices));
    knowncommands.add(movecsys);

    // Create list of systems we wish the actor template to have.
    std::vector<std::wstring> systemnames;
    // Tell it we want the actor to possess the 'position' system.
    // systemnames.push_back( possys->name() );

    std::vector<std::wstring> commandnames;
    // Tell it we want the actor to have the 'move' command.
    commandnames.push_back(movecsys->name());

    idallocator_ptr actor_ids(new idallocator());

    actortemplate_ptr at = actortemplate::create(
        systemnames, commandnames, knownsystems, knowncommands, nameIndices, actor_ids);

    return at;
}

/*
    *
    */
class localplayer : public solosnake::player
{
public:
    localplayer(commandtranslator_ptr translator, const_state_ptr state)
        : solosnake::player(1 + state->get_step()), state_(state), translator_(translator)
    {
    }

    void do_frame_processing()
    {
        this->add_command(get_action());
    }

private:
    command get_action()
    {
        const state& s = *state_;
        return command(s[0]->id(), s[0]->id(), 0);
    }

    const_state_ptr state_;
    commandtranslator_ptr translator_;
};

/*
    *
    */
class networkedplayer : public solosnake::player
{
public:
    explicit networkedplayer(commandtranslator_ptr translator, const_state_ptr state)
        : solosnake::player(state->get_step())
        , translated_commands_(state->get_step())
        , translator_(translator)
    {
    }

    void set_frame_messages(const bytebuffer& msg)
    {
        translator_->translate_to_commands(msg, translated_commands_);

        if (translated_commands_.get_step() == get_step())
        {
            this->add_commands(translated_commands_);
        }
    }

private:
    commandlist translated_commands_;
    commandtranslator_ptr translator_;
};

/*
    *
    */
class minigamebroadcaster : public solosnake::broadcaster
{
public:
    explicit minigamebroadcaster(const_state_ptr state) : commands_(state->get_step())
    {
    }

    virtual void broadcast(const commandlist& commands)
    {
        if (commands_.is_empty())
        {
            commands_ = commands;
        }
        else
        {
            commands_ += commands;
        }
    }

    virtual bool all_have_been_broadcast() const
    {
        return commands_.is_empty();
    }

    const commandlist& get_commands() const
    {
        return commands_;
    }

    void clear_commands()
    {
        commands_.clear();
    }

private:
    commandlist commands_;
};

typedef std::tr1::shared_ptr<localplayer> localplayer_ptr;
typedef std::tr1::shared_ptr<networkedplayer> networkedplayer_ptr;
typedef std::tr1::shared_ptr<minigamebroadcaster> minigamebroadcaster_ptr;

/*
    *
    */
class minigame : public solosnake::network::lockstepserver::igame
{
public:
    minigame(size_t max_game_steps, size_t expectedParticipantsCount)
        : max_game_steps_(max_game_steps)
    {
        state_ptr state = state::create_state();

        commandtranslator_ptr translator(new commandtranslator());

        actortemplate_ptr at = create_test_system();

        // Create some actors.
        for (size_t i = 0; i < 20; ++i)
        {
            actor_ptr a = state->create_and_add_actor(at);
        }

        mainplayer_.reset(new localplayer(translator, state));

        players otherplayers;

        for (size_t i = 1; i < expectedParticipantsCount; ++i)
        {
            networkedplayer_ptr p(new networkedplayer(translator, state));
            otherplayers.push_back(p);
            networkplayers_.push_back(p);
        }

        participants_ptr allplayers(new participants(mainplayer_, otherplayers));

        broadcaster_.reset(new minigamebroadcaster(state));

        long step_interval = 1000 / 10;

        match_.reset(new match(state, allplayers, broadcaster_, step_interval));
    }

    virtual ~minigame()
    {
    }

    virtual void initial_connecting_update()
    {
    }

    virtual void connecting_update(long milliseconds)
    {
    }

    virtual bool initial_frame_update()
    {
        return true;
    }

    virtual bool frame_update(long milliseconds)
    {
        mainplayer_->do_frame_processing();
        match_->advance();
        return match_->get_step() < max_game_steps_;
    }

    virtual void get_frame_message(bytebuffer& msg)
    {
        if (broadcaster_->get_commands().count() > 0)
        {
            commandtranslator t;

            msg = t.translate_to_message(*broadcaster_->get_commands().begin());

            broadcaster_->clear_commands();
        }
    }

    virtual void insert_frame_messages(const std::vector<bytebuffer>& messages)
    {
        EXPECT_TRUE(messages.size() == networkplayers_.size());
        for (size_t i = 0; i < messages.size(); ++i)
        {
            if (!messages[i].is_empty())
            {
                networkplayers_[i]->set_frame_messages(messages[i]);
            }
            else
            {
                //    assert(0);
            }
        }
    }

private:
    match_ptr match_;
    size_t max_game_steps_;
    std::vector<networkedplayer_ptr> networkplayers_;
    localplayer_ptr mainplayer_;
    minigamebroadcaster_ptr broadcaster_;
};

void start_host(size_t n_participants, size_t max_steps, std::ostream* log)
{
    std::tr1::shared_ptr<minigame> game(new minigame(max_steps, n_participants));
    std::tr1::shared_ptr<solosnake::network::lockstepserver> host(
        new solosnake::network::lockstepserver(game, n_participants, Port, log));
}

void start_client(size_t n_participants, size_t max_steps, std::ostream* log)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    std::tr1::shared_ptr<minigame> game(new minigame(max_steps, n_participants));
    std::tr1::shared_ptr<solosnake::network::lockstepserver> client(
        new solosnake::network::lockstepserver(game, n_participants, "127.0.0.1", Port, log));
}
}

void run_n_clients(size_t n, bool log)
{
    {
        const size_t max_steps = 100;
        const size_t ExpectedParticipantsCount = n;

        boost::thread_group group;

        std::stringstream ss;
        ss << "run_" << n << "_clients_";
        std::string s = ss.str();

        std::vector<std::tr1::shared_ptr<std::ofstream>> files;

        if (log)
        {
            files.push_back(std::tr1::shared_ptr
                            <std::ofstream>(new std::ofstream((s + "host.txt").c_str())));
        }

        group.add_thread(new boost::thread(
            boost::bind(start_host,
                        ExpectedParticipantsCount,
                        max_steps,
                        log ? files[0].get() : reinterpret_cast<std::ostream*>(0))));

        for (size_t i = 2; i < ExpectedParticipantsCount; ++i)
        {
            if (log)
            {
                std::stringstream oss;
                oss << s << i << "_client.txt";
                files.push_back(std::tr1::shared_ptr
                                <std::ofstream>(new std::ofstream(oss.str().c_str())));
            }

            group.add_thread(new boost::thread(boost::bind(
                start_client,
                ExpectedParticipantsCount,
                max_steps,
                log ? files[files.size() - 1].get() : reinterpret_cast<std::ostream*>(0))));
        }

        if (log)
        {
            files.push_back(std::tr1::shared_ptr
                            <std::ofstream>(new std::ofstream((s + "_main_client.txt").c_str())));
        }

        start_client(ExpectedParticipantsCount,
                     max_steps,
                     log ? files[files.size() - 1].get() : reinterpret_cast<std::ostream*>(0));

        group.join_all();
    }
}

TEST(minigame, three_clients_logging)
{
    {
        run_n_clients(3, true);
    }
}

TEST(minigame, four_clients_logging)
{
    {
        run_n_clients(4, true);
    }
}

TEST(minigame, two_clients_no_logging)
{
    {
        run_n_clients(2, false);
    }
}
