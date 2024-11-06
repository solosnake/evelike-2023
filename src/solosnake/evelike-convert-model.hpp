#ifndef SOLOSNAKE_EVELIKE_CONVERT_MODEL_HPP
#define SOLOSNAKE_EVELIKE_CONVERT_MODEL_HPP

#include <string_view>

namespace solosnake::evelike
{
    int convert_model(const std::string_view& src,
                      const std::string_view& dest,
                      bool debugging,
                      bool show_model);
}

#endif
