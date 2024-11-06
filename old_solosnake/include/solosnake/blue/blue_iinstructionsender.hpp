#ifndef blue_iinstructionsender_hpp
#define blue_iinstructionsender_hpp

namespace blue
{
    // Interface for object which sends instructions to the pending
    // instructions object. The pending instructions object will keep it
    // informed of the state of its pending instructions (aborted, used).
    class iinstructionsender
    {
    public:

        virtual ~iinstructionsender();

        virtual void instructions_were_aborted() = 0;

        virtual void instructions_were_sent_to_board() = 0;
    };
}

#endif
