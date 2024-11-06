#ifndef SOLOSNAKE_EVELIKE_ICON_HPP
#define SOLOSNAKE_EVELIKE_ICON_HPP

#include <optional>
#include "solosnake/image.hpp"

namespace solosnake::evelike
{
    std::optional<Image> get_evelike_window_icon_image() noexcept;
}

#endif