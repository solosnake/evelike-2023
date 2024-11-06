#ifndef solosnake_igameloop_hpp
#define solosnake_igameloop_hpp

#include <memory>
#include "solosnake/loopresult.hpp"

namespace solosnake
{
    //! A game loop class compatible with the needs of the server architecture.
    class igameloop
    {
    public:

        virtual ~igameloop();

        //! Guaranteed to be called once, and before any frame updates.
        virtual void       initial_preframe_update()                                = 0;

        //! Optional, called with delta (ms) since last preframe call.
        virtual LoopResult preframe_update( const unsigned long milliseconds )      = 0;

        //! If 'preframe_update' was called and returned 'LoopAgain',
        //! this will be called once. This is frame zero.
        virtual LoopResult initial_frame_update()                                   = 0;

        //! Continuously called during loop with the delta since
        //! last 'frame_update' in ms.
        virtual LoopResult frame_update( const unsigned long milliseconds )         = 0;

        //! Advances the game state one tick / frame.
        virtual void advance_game_one_frame()                                       = 0;

        //! The application is killing the update loop.
        virtual void       final_frame_update( const unsigned long milliseconds )   = 0;
    };
}

#endif
