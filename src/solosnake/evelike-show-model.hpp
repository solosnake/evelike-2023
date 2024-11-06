#ifndef SOLOSNAKE_EVELIKE_SHOW_MODEL_HPP
#define SOLOSNAKE_EVELIKE_SHOW_MODEL_HPP

#include <string_view>

namespace solosnake::evelike
{
    int show_model(const std::string_view& model_file,
                   const std::string_view& skybox_file,
                   bool debugging,
                   std::uint64_t max_milliseconds);
}

#endif
