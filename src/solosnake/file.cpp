#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "solosnake/file.hpp"

namespace solosnake
{
    std::string get_file_contents(const std::string_view& file)
    {
        std::ifstream ifs(file.data());
        if (ifs)
        {
            return std::string((std::istreambuf_iterator<char>(ifs)),
                               (std::istreambuf_iterator<char>()));
        }
        else
        {
            std::stringstream ss;
            ss << "Failed to open '" << file << "'.";
            throw std::runtime_error(ss.str());
        }
    }
}