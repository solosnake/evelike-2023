#ifndef blue_emptygame_hpp
#define blue_emptygame_hpp

#include "solosnake/blue/blue_game.hpp"

namespace blue 
{
    class emptygame : public game 
    {
    public:

        emptygame( 
            const std::string& xmlScreenFile,
            const std::shared_ptr<blue_user_settings>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const game::HeapOnly&);

        virtual ~emptygame();

        virtual bool try_load_game(const std::shared_ptr<gamestartupargs>&);
    };
}

#endif
