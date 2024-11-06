#include <iostream>
#include "solosnake/blue/blue_gameaction.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_instruction.hpp"
#include "solosnake/blue/blue_igameactions.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/unaliased.hpp"

using namespace std;

namespace blue
{
    namespace
    {
        enum GameActions
        {
            SendInstructionsToBoardGameAction = 1
        };

#       define SS_ACTIONRANGE_BEGIN (SendInstructionsToBoardGameAction)
#       define SS_ACTIONRANGE_END (SendInstructionsToBoardGameAction + 1)

        GameActions get_gameaction( const solosnake::network::byte* p );

        size_t get_message_size( const solosnake::network::byte* p );

        // Transforms GameActions enum to byte.
        inline solosnake::network::byte get_gamaction_byte( GameActions action )
        {
            return static_cast<solosnake::network::byte>( action );
        }

        // Returns GameAction from byte buffer pointer.
        GameActions get_gameaction( const solosnake::network::byte* p )
        {
            if( ( *p ) < SS_ACTIONRANGE_BEGIN || ( *p ) >= SS_ACTIONRANGE_END )
            {
                ss_throw( "Invalid gameaction network buffer byte 0" );
            }

            return static_cast<GameActions>( *p );
        }

        struct SendInstructionsToBoardBuffer
        {
            solosnake::network::byte actionType; // Must always be first byte!!
            uint8_t coordX;
            uint8_t coordY;
            uint8_t padding0;
            uint32_t instructionsCount;
        };

        struct InstructionBuffer
        {
            uint16_t data[4];
        };

        inline InstructionBuffer make_instruction_buffer( const Instruction& i )
        {
            InstructionBuffer ib;
            ib.data[0] = i[0];
            ib.data[1] = i[1];
            ib.data[2] = i[2];
            ib.data[3] = i[3];
            return ib;
        }

        static_assert( sizeof( SendInstructionsToBoardBuffer ) == ( 1 + 1 + 1 + 1 + 4 ),
                       "Unexpected size of SendInstructionsToBoardBuffer: packing "
                       "may differ." );

        static_assert( sizeof( InstructionBuffer ) == ( 8 ),
                       "Unexpected size of InstructionBuffer: packing may differ." );

        // Returns the total size of the buffer that follows this game action buffer
        // head byte, including it.
        size_t get_message_size( const solosnake::network::byte* p )
        {
            size_t buffSize = 0u;

            switch( get_gameaction( p ) )
            {
                case SendInstructionsToBoardGameAction:
                    buffSize = sizeof( SendInstructionsToBoardBuffer )
                               + ( sizeof( InstructionBuffer )
                                   * reinterpret_cast
                                   <const SendInstructionsToBoardBuffer*>( p )->instructionsCount );
                    break;
            }

            return buffSize;
        }
    }

    gameaction::gameaction( const solosnake::network::byte* bytes )
        : action_as_buffer_( bytes, get_message_size( bytes ) )
    {
    }

    void gameaction::do_action( igameactions& availableActions ) const
    {
        const solosnake::network::byte* bytes = action_as_buffer_.data();

        switch( get_gameaction( bytes ) )
        {
            case SendInstructionsToBoardGameAction:
            {
                const SendInstructionsToBoardBuffer* header =
                    reinterpret_cast<const SendInstructionsToBoardBuffer*>( bytes );

                const std::uint16_t* codeOps =
                    reinterpret_cast<const std::uint16_t*>( bytes + sizeof( SendInstructionsToBoardBuffer ) );

                assert( header->instructionsCount > 0 );

                const Hex_coord xy = Hex_coord::make_coord( header->coordX, header->coordY );

                auto code = make_shared<Instructions>(
                    Instruction::asm_to_instructions( codeOps, header->instructionsCount ) );

                availableActions.send_instructions_to_board( xy, code );
            }
            break;
        }
    }

    gameaction gameaction::make_send_instructions_to_board( Hex_coord xy, const Instructions& code )
    {
        vector<solosnake::network::byte> buffer;

        const size_t insCount = code.size();
        const size_t bufferSize = sizeof( SendInstructionsToBoardBuffer )
                                  + ( sizeof( InstructionBuffer ) * insCount );

        buffer.resize( bufferSize );

        SendInstructionsToBoardBuffer* unaliased header = reinterpret_cast
                <SendInstructionsToBoardBuffer*>( buffer.data() );
        InstructionBuffer* unaliased instructionsBuffer = reinterpret_cast
                <InstructionBuffer*>( buffer.data() + sizeof( SendInstructionsToBoardBuffer ) );

        header->actionType = get_gamaction_byte( SendInstructionsToBoardGameAction );
        header->coordX = xy.x;
        header->coordY = xy.y;
        header->instructionsCount = static_cast<uint32_t>( insCount );

        for( size_t i = 0; i < insCount; ++i )
        {
            // i+1 because instructions use 1 based indexing.
            instructionsBuffer[i] = make_instruction_buffer( code[i + 1] );
        }

        return gameaction( move( buffer ) );
    }
}
