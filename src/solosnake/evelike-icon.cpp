#include "solosnake/evelike-icon.hpp"

namespace solosnake::evelike
{
    std::optional<Image> get_evelike_window_icon_image() noexcept
    {
        auto width = 48u;
        auto height = 48u;
        auto pixels = std::vector<Bgra>(width * height, Bgra{ 255, 0, 0, 255 });
        Image img(width, pixels);
        return img;
    }
}