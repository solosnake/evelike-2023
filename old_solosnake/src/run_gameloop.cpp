#include "solosnake/igameloop.hpp"
#include "solosnake/run_gameloop.hpp"
#include "solosnake/timer.hpp"

namespace solosnake
{
    // This loop has the same characteristics as the networked game loop
    // in terms of call ordering etc.
    void run_gameloop( igameloop& game, unsigned long tickLengthMs )
    {
        bool want_main_loop = true;

        game.initial_preframe_update();

        const unsigned long duration_of_gametick_ms_ = tickLengthMs;
        bool run_loop = want_main_loop && LoopAgain == game.initial_frame_update();
        bool error_free_ = true;
        unsigned long frame_counter = 1;

        auto timing = make_timer();
        timing->restart();

        while (run_loop && error_free_)
        {
            // Begin a Frame
            timing->restart();

            const unsigned long timeAtStartOfUpdateLoop = timing->elapsed_milliseconds();
            unsigned long timeSinceLoopBegan = 0;
            unsigned long timeOfLastLoop = timeAtStartOfUpdateLoop;

            do
            {
                const unsigned long timeNow = timing->elapsed_milliseconds();
                timeSinceLoopBegan = timeNow - timeAtStartOfUpdateLoop;
                if (timeNow > timeOfLastLoop)
                {
                    run_loop = LoopAgain == game.frame_update(timeNow - timeOfLastLoop);
                    timeOfLastLoop = timeNow;
                }
            } 
            while ( error_free_ && run_loop && timeSinceLoopBegan < duration_of_gametick_ms_ );

            if (run_loop && error_free_)
            {
                ++frame_counter;
                game.advance_game_one_frame();
            }

            timing->restart();
        }

        game.final_frame_update(timing->elapsed_milliseconds());
    }
}

