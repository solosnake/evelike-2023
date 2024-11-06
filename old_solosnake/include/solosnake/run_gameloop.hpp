#ifndef solosnake_run_gameloop_hpp
#define solosnake_run_gameloop_hpp

namespace solosnake 
{
    class igameloop;

    //!  Runs a game loop in a similar manner to the networked game loop.
    //! @param tickLengthMs How many milliseconds long a frame tick is considered to be.
    void run_gameloop( igameloop&, unsigned long tickLengthMs );
}

#endif
