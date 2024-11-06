#ifndef blue_compilationresult_hpp
#define blue_compilationresult_hpp

#include <string>
#include "solosnake/blue/blue_instruction.hpp"

namespace blue
{
    /// Returns the results of a compilation of a single line of code.
    /// A successful compilation may have warnings, but will never have
    /// errors. In the event of an error the result will throw if the
    /// Instruction is accessed.
    class Compilation_result
    {
    public:

        /// Constructs a successful compilation result for this Instruction.
        /// No examination of the Instruction is done.
        Compilation_result( const Instruction& ) noexcept;

        /// Constructs a successful compilation result for this Instruction,
        /// but with a warning. No examination of the Instruction is done.
        Compilation_result( const Instruction&, const std::string_view& warning_text );

        /// Constructs a failed compilation result with the given error.
        /// Throws if the error string is empty.
        explicit Compilation_result( const std::string_view& err );

        /// Returns true only if this compilation result was successful (i.e.
        /// has no error.
        bool was_successful() const noexcept;

        /// Throws if this compilation result was an error.
        const Instruction& get_instruction() const;

        /// Returns the (possibly empty) error text.
        std::string_view error_text() const noexcept;

        /// Returns the (possibly empty) warning text.
        std::string_view warning_text() const noexcept;

    private:

        std::string error_;
        std::string warning_;
        Instruction instruction_;
    };
}

#include "solosnake/blue/blue_compilation_result.inl"
#endif
