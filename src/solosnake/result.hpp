#ifndef SOLOSNAKE_RESULT_HPP
#define SOLOSNAKE_RESULT_HPP

#include <string_view>

namespace solosnake
{
    class Result
    {
    public:

        constexpr Result() noexcept;

        constexpr Result( bool result ) noexcept;

        explicit constexpr Result( const char msg[] ) noexcept;

        explicit constexpr Result( std::string_view error ) noexcept;

        operator bool () const noexcept;

        bool succeeded() const noexcept;

        bool failed() const noexcept;

        std::string_view error() const noexcept;

    private:

        constexpr static std::string_view FAIL_MESSAGE = "FAILED";

        std::string_view error_;
    };
}

#include "solosnake/result.inl"
#endif