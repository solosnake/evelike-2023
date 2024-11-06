#ifndef SOLOSNAKE_FILE_HPP
#define SOLOSNAKE_FILE_HPP

#include <string>
#include <string_view>

namespace solosnake
{
    /// Opens, reads and returns the contents of the named file as a string,
    /// else throws.
    std::string get_file_contents(const std::string_view& file);
}

#endif