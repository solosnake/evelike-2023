#ifndef blue_gameaction_hpp
#define blue_gameaction_hpp

#include "solosnake/byte.hpp"
#include "solosnake/bytebuffer.hpp"

namespace blue
{
    class igameactions;
    class Instructions;
    struct Hex_coord;

    //! A gameaction object represents a parameterized call to a igameactions
    //! object to change the game state. A gameaction can only 'do' what is
    //! permitted by the igameactions interface. It is a means of serializing
    //! an action and its parameters. A gameaction is not changed by applying
    //! it to the  game state via the igameactions interface.
    //! Actions are translated into a bytebuffer msg which is passed back to the
    //! server. The server will then later call the process_commands which will
    //! contain our own command, which will be processed.
    class gameaction
    {
    public:

        gameaction();

        gameaction( const gameaction& );

        gameaction& operator=( const gameaction& );

        gameaction( gameaction&& );

        gameaction& operator=( gameaction && );

        explicit gameaction( const solosnake::network::byte* );

        explicit gameaction( std::vector<solosnake::network::byte>&& );

        // How many bytes this object is when serialized. Identical to how
        //  many bytes used in construction of this object.
        size_t bytes_size() const;

        //! Applies the action represented by this object to the igameactions
        //! object. The action might be parameterized by the contents of this
        //! object. A gameaction can only 'do' what is permitted by the
        //! igameactions interface.
        void do_action( igameactions& ) const;

        const solosnake::network::bytebuffer& to_bytebuffer() const;

        static gameaction make_send_instructions_to_board( Hex_coord, const Instructions& code );

    private:

        solosnake::network::bytebuffer action_as_buffer_;
    };
}

#include "solosnake/blue/blue_gameaction.inl"

#endif
