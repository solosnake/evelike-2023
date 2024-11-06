#include <algorithm>
#include <regex>
#include "solosnake/blue/blue_machine_name.hpp"
#include "solosnake/blue/blue_throw.hpp"

namespace blue
{
    Machine_name::Machine_name(std::uint64_t id)
    {
        ss_throw("Not implemented");
    }

    Machine_name::Machine_name(const char* name) : Machine_name( std::string_view(name) )
    {
    }

    Machine_name::Machine_name(const std::string_view& name)
    {
        static const std::regex rg_name("^[a-zA-Z\\d]([\\-\\.]?[a-zA-Z\\d]){0,6}$");

        if (! std::regex_match(name.data(), rg_name))
        {
            ss_throw("Illegal Machine_name.");
        }

        const auto len = std::min<std::size_t>(name.length(), 7U);
        for(auto i=0U; i < len; ++i)
        {
            text_[i] = name[i];
        }
    }
}
