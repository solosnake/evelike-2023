#ifndef blue_example_game_hpp
#define blue_example_game_hpp

#include "solosnake/blue/blue_game.hpp"

namespace blue 
{
    class examplegame : public game 
    {
    public:

        examplegame( 
            const std::string& xmlScreenFile,
            const std::shared_ptr<blue_user_settings>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const game::HeapOnly&);

        virtual ~examplegame();

        virtual bool try_load_game(const std::shared_ptr<gamestartupargs>&);
    };
}

#endif
