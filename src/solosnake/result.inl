#include "solosnake/result.hpp"

namespace solosnake
{
    inline Result::operator bool () const noexcept
    {
        return succeeded();
    }

    inline bool Result::succeeded() const noexcept
    {
        return error_.empty();
    }

    inline bool Result::failed() const noexcept
    {
        return not succeeded();
    }

    inline std::string_view Result::error() const noexcept
    {
        return error_;
    }

    inline constexpr Result::Result() noexcept
        : error_(Result::FAIL_MESSAGE)
    {
    }

    inline constexpr Result::Result(const bool result) noexcept
        : error_( result ? std::string_view() : Result::FAIL_MESSAGE)
    {
    }

    inline constexpr Result::Result(const char msg[]) noexcept
        : Result( std::string_view(msg) )
    {
    }

    inline constexpr Result::Result( std::string_view msg ) noexcept
        : error_(msg.empty() ? Result::FAIL_MESSAGE : msg )
    {
    }
}
