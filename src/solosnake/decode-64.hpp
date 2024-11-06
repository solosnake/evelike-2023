#ifndef SOLOSNAKE_DECODE_64_HPP
#define SOLOSNAKE_DECODE_64_HPP

#include <cstdint>
#include <string_view>
#include <vector>

namespace solosnake
{
    std::vector<std::uint8_t> decode_64(const std::string_view&);
}
#endif
