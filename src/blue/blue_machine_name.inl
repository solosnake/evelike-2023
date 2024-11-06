#include "solosnake/blue/blue_machine_name.hpp"

namespace blue
{
    inline Machine_name::operator std::string_view () const noexcept
    {
        return std::string_view(text_.data());
    }

    inline Machine_name::operator const char* () const noexcept
    {
        return text_.data();
    }

    inline const char* Machine_name::c_str() const noexcept
    {
        return text_.data();
    }

    inline char Machine_name::operator[]( size_t n ) const noexcept
    {
        return text_[n];
    }

    inline bool Machine_name::operator == (const Machine_name& rhs) const noexcept
    {
        return text_ == rhs.text_;
    }

    inline bool Machine_name::operator != (const Machine_name& rhs) const noexcept
    {
        return text_ != rhs.text_;
    }
}
