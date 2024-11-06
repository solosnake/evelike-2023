#ifndef solosnake_guicommandexecutor_hpp
#define solosnake_guicommandexecutor_hpp
#include "solosnake/iscreenstate.hpp"

namespace solosnake
{
    class guicommandexecutor : public iscreenstate
    {
    public:

    private:
        virtual bool is_game_over() const;
    };
}

#endif
