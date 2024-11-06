#ifndef SOLOSNAKE_EVELIKE_CONVERT_SKYBOX_HPP
#define SOLOSNAKE_EVELIKE_CONVERT_SKYBOX_HPP

#include <string_view>

namespace solosnake::evelike
{
    int convert_skybox(const std::string_view& src,
                       const std::string_view& xp,
                       const std::string_view& xn,
                       const std::string_view& yp,
                       const std::string_view& yn,
                       const std::string_view& zp,
                       const std::string_view& zn,
                       bool debugging,
                       bool show_model);
}

#endif
