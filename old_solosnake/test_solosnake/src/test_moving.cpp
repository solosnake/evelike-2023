#include "solosnake/testing/testing.hpp"
#include <stdio.h>
#include <crtdbg.h>
#include "namemapper.hpp"
#define _CRTDBG_MAP_ALLOC

using namespace solosnake;

namespace
{
class ability
{
public:
};

class moveto : public ability
{
private:
    size_t x;
    size_t y;
    size_t dx;
    size_t dy;

public:
    moveto(std::wtring name, namemapper& lookup)
    {
        x = lookup.get_index(L"x");
        y = lookup.get_index(L"y");
        dx = lookup.get_index(L"destz");
        dy = lookup.get_index(L"desty");
    }
};
}

TEST(moving, moving)
{
    {
        std::vector<std::wstring> commands;
        commands.push_back(L"moveto");
        commands.push_back(L"attack");
        commands.push_back(L"defend");
        commands.push_back(L"heal");

        std::vector<std::wstring> systems;
        systems.push_back(L"repairs");
        systems.push_back(L"jammable");
        systems.push_back(L"attackable");

        std::vector<std::wstring> exposures;
        exposures.push_back(L"healable");
        exposures.push_back(L"jammable");
        exposures.push_back(L"attackable");

        actortemplate at(known_commands.commands(commands),
                         known_systems.systems(systems),
                         known_exposures.exposures(exposures));
    }
}
