#include "solosnake/blue/blue_compilation_result.hpp"

namespace blue
{
    inline Compilation_result::Compilation_result( const Instruction& i ) noexcept
        : error_()
        , warning_()
        , instruction_( i )
    {
    }

    inline Compilation_result::Compilation_result( const Instruction& i, const std::string_view& warning_text )
        : error_()
        , warning_( warning_text )
        , instruction_( i )
    {
    }

    inline bool Compilation_result::was_successful() const noexcept
    {
        return error_.empty();
    }

    inline std::string_view Compilation_result::error_text() const noexcept
    {
        return error_;
    }

    inline std::string_view Compilation_result::warning_text() const noexcept
    {
        return warning_;
    }
}
