#ifndef blue_mainmenugame_hpp
#define blue_mainmenugame_hpp

#include "solosnake/blue/blue_game.hpp"

namespace blue 
{
    class mainmenugame : public game 
    {
    public:

        mainmenugame( 
            const std::string& xmlScreenFile,
            const std::shared_ptr<blue_user_settings>&,
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const std::shared_ptr<datapaths>&,
            const std::shared_ptr<solosnake::ilanguagetext>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const game::HeapOnly&);

        virtual ~mainmenugame();
        
        virtual bool try_load_game(const std::shared_ptr<gamestartupargs>&);
    };
}

#endif
