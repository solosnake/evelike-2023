#ifndef blue_ipendinginstructions_hpp
#define blue_ipendinginstructions_hpp

#include <memory>

namespace blue
{
    class Instructions;
    class iinstructionsender;

    //! Mediator between the game/board and the gui widgets. The widgets
    //! send instructions to here which the user can send to the board
    //! by clicking on it.
    class ipendinginstructions
    {
    public:

        virtual ~ipendinginstructions();

        //! Sets these instructions as instructions which will be sent
        //! to board when user interacts with it.
        virtual void set_pending_instructions(
            iinstructionsender*,
            const std::shared_ptr<Instructions>& ) = 0;

        //! When the iinstructionsender goes out of scope it removes the pointer
        //! to it.
        virtual void cancel_pending_instructions_for(
            const iinstructionsender* ) = 0;
    };
}

#endif
