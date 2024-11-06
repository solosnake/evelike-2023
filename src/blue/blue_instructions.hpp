#ifndef blue_instructions_hpp
#define blue_instructions_hpp

#include <string_view>
#include <vector>
#include "solosnake/blue/blue_instruction.hpp"

namespace blue
{
    /// Container for the instructions for a Machine, this handles loading and
    /// saving and also makes it impossible to reference an Instruction out of
    /// range. Also it is impossible to change the size of the internal
    /// container.
    /// Instructions use 1 based indexing, to simplify the writing of scripts
    /// using editors with line numbering. Accessing the instructions uses
    /// modulo arithmetic and is never out of bounds. Zero wraps to the last
    /// Instruction.
    class Instructions
    {
    public:

        Instructions() = default;

        explicit Instructions( const std::string_view& );

        explicit Instructions( const std::vector<Instruction>& );

        explicit Instructions( std::vector<Instruction>&& );

        /// 1-based modulo arithmetic indexing. `1` will returns the
        /// 0th element of the vector.
        Instruction& operator[]( size_t n );

        /// 1-based modulo arithmetic indexing. `1` will returns the
        /// 0th element of the vector.
        const Instruction& operator[]( size_t n ) const;

        /// Returns the numer of instructions in this object.
        std::uint16_t size() const noexcept;

        ///  Swaps the contents of this object with that of `rhs`.
        void swap( Instructions& rhs ) noexcept;

        /// Throws if the subset formed would be empty.
        Instructions subset( size_t start, size_t n ) const;

        bool operator==( const Instructions& rhs ) const noexcept;
        bool operator!=( const Instructions& rhs ) const noexcept;

    private:

        friend class to_from_json;

        std::vector<Instruction> instructions_ { Instruction() };
    };
}

#include "solosnake/blue/blue_instructions.inl"
#endif
