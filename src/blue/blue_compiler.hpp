#ifndef blue_compiler_hpp
#define blue_compiler_hpp

#include <string>
#include <string_view>
#include <vector>
#include "solosnake/blue/blue_compilation_result.hpp"
#include "solosnake/blue/blue_instructions.hpp"

namespace blue
{
    /// Compiles an array of code lines to an array of Instructions.
    /// Throws if any of the lines are invalid.
    Instructions compile_code( const std::vector<std::string>& );

    /// Compiles a string of code separated by '\n' to an array of Instructions.
    /// Throws if any of the lines are invalid.
    Instructions compile_code( const std::string_view& );

    /// Compiles a line of code @a line to an Instruction, throwing if @a line
    /// is not valid code.
    Instruction compile( const std::string_view& line );

    /// Tries to compile the given line of code @a line. The returned
    /// Compilation_result must be queried to see if compilation was successful.
    /// In the event of an unsuccessful compilation, the return value will
    /// contain the reason for failure as a string.
    /// @note A successfully compiled line of code may contain warnings.
    Compilation_result try_compile( const std::string_view& line );

    /// Returns the corresponding line of code for this Instruction.
    std::string decompile( const Instruction& );

    /// Returns a (possibly empty) vector of code strings that this snippet of
    /// text could expand to. The returned strings are statically allocated
    /// within the internal implementation of the compiler.
    std::vector<const char*> predict_text( const char* );
}

#endif
