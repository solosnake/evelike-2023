#ifndef blue_pendinginstructions_hpp
#define blue_pendinginstructions_hpp

#include "solosnake/blue/blue_ipendinginstructions.hpp"

namespace blue
{
    //! Mediator between the game/board and the gui widgets. The widgets
    //! send instructions to here which the user can send to the board
    //! by clicking on it.
    class pendinginstructions : public ipendinginstructions
    {
    public:

        pendinginstructions();

        virtual ~pendinginstructions();

        //! Returns true if there are pending instructions.
        bool has_instructions() const;

        //! Returns any pending instructions and notifies the original sender
        //! they were sent to board. Clears the pending instructions.
        std::shared_ptr<Instructions> take_instructions();

    private:

        void set_pending_instructions(
            iinstructionsender*,
            const std::shared_ptr<Instructions>& ) override;

        void cancel_pending_instructions_for(
            const iinstructionsender* ) override;

    public:

        std::shared_ptr<Instructions>   pending_;
        iinstructionsender*             pending_sender_;
    };
}

#endif
