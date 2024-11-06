#ifndef solosnake_loopresult_hpp
#define solosnake_loopresult_hpp

namespace solosnake
{
    //! Return code when querying a loop - returns whether loop should be
    //! stopped or run again.
    enum LoopResult
    {
        StopLooping = 0,
        LoopAgain = 1
    };
}

#endif
