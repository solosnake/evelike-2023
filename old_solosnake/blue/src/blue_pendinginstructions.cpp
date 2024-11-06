#include <cassert>
#include "solosnake/blue/blue_pendinginstructions.hpp"
#include "solosnake/blue/blue_iinstructionsender.hpp"
#include "solosnake/blue/blue_instructions.hpp"

namespace blue
{
    pendinginstructions::pendinginstructions() : pending_sender_( nullptr )
    {
    }

    pendinginstructions::~pendinginstructions()
    {
        if( pending_sender_ )
        {
            pending_sender_->instructions_were_aborted();
            pending_sender_ = nullptr;
        }
    }

    void pendinginstructions::set_pending_instructions(
            iinstructionsender* sender,
            const std::shared_ptr<instructions>& newinstructions )
    {
        assert( sender );
        assert( newinstructions );

        if( pending_sender_ && ( pending_sender_ != sender ) )
        {
            // New sender. Tell old one its instructions were not used.
            assert( pending_sender_ );
            pending_sender_->instructions_were_aborted();
        }

        pending_ = newinstructions;
        pending_sender_ = sender;
    }

    void pendinginstructions::cancel_pending_instructions_for( const iinstructionsender* sender )
    {
        if( pending_sender_ == sender )
        {
            pending_sender_ = nullptr;
            pending_.reset();
        }
    }

    bool pendinginstructions::has_instructions() const
    {
        return pending_sender_ ? true : false;
    }

    std::shared_ptr<instructions> pendinginstructions::take_instructions()
    {
        std::shared_ptr<instructions> result;

        if( pending_sender_ )
        {
            result.swap( pending_ );
            pending_sender_->instructions_were_sent_to_board();
            pending_sender_ = nullptr;
        }

        return result;
    }
}
