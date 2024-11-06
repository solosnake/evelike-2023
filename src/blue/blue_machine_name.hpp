#ifndef blue_machine_name_hpp
#define blue_machine_name_hpp

#include <array>
#include <string_view>

namespace blue
{
    /// Restricts a Machine's name to a 7 character string, which
    /// must begin with a letter or number, and may contain '-' or '.'
    /// e.g. 'ABC-123', 'A-12345', '0', 'Zzz', 'a45X' etc.
    /// Ignoring case and '-', there are ~78364164095 numbers between 0000000
    /// and ZZZZZZZ, or 0x123EDE3FFF permutations. This requires a unsigned 64
    /// bit value to produce a valid conversion.
    class Machine_name
    {
    public:

        Machine_name() = default;

        Machine_name(const std::string_view&);

        Machine_name(const char*);

        /// Converts the number to a name. 0 becomes 0000000.
        /// Throws if the number is beyond what can be uniquely represented
        /// as a name.
        explicit Machine_name(std::uint64_t id);

        operator std::string_view () const noexcept;

        operator const char* () const noexcept;

        const char* c_str() const noexcept;

        char operator[]( size_t n ) const noexcept;

        bool operator == (const Machine_name& rhs) const noexcept;
        bool operator != (const Machine_name& rhs) const noexcept;

    private:

        std::array<char, 8U> text_ = {};
    };
}

#include "solosnake/blue/blue_machine_name.inl"
#endif