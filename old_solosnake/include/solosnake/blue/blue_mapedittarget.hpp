#ifndef blue_mapedittarget_hpp
#define blue_mapedittarget_hpp

#include <vector>
#include "solosnake/blue/blue_asteroid.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_sun.hpp"

namespace blue
{
    //! This is the target upon which the mapedit commands act. All the
    //! member data is intentionally public as this is intended to be purely
    //! a data set.
    struct mapedittarget
    {
        mapedittarget();

        Hex_grid                     grid_;
        std::vector<Asteroid>       asteroids_;
        std::vector<Sun>            suns_;
    };
}

#endif
