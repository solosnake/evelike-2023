#include "solosnake/testing/testing.hpp"
#include "solosnake/game/state.hpp"
#include "solosnake/game/actor.hpp"

using namespace solosnake::game;

namespace
{

actortemplate_ptr new_actortemplate_ptr()
{
    std::vector<std::wstring> systemnames;
    factories fctry;
    knownsystemfactories allknown(fctry);
    namemapper_ptr nameIndices(new namemapper());
    idallocator_ptr actor_ids(new idallocator());

    actortemplate_ptr at = actortemplate::create(systemnames, allknown, nameIndices, actor_ids);

    return at;
}
}

TEST(state, ctor)
{
    {
        state_ptr s = state::create_state();
    }
}

TEST(state, actor_count_zero)
{
    {
        state_ptr s = state::create_state();
        EXPECT_TRUE(s->actor_count() == 0);
    }
}

TEST(state, actor_count_correct)
{
    {
        actortemplate_ptr atp = new_actortemplate_ptr();
        state_ptr s = state::create_state();
        EXPECT_TRUE(s->actor_count() == 0);
        s->create_and_add_actor(atp);
        EXPECT_TRUE(s->actor_count() == 1);
        s->create_and_add_actor(atp);
        EXPECT_TRUE(s->actor_count() == 2);
    }
}