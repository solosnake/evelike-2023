#ifndef blue_load_scene_suns_hpp
#define blue_load_scene_suns_hpp

// TODO RENAME THIS FILE

#include <array>
#include "solosnake/itextureloader.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/blue/blue_isun_src.hpp"

namespace blue
{
    //! Opens the named file and reads in the descriptions of the settings to
    //! use for the sun types.
    std::array<scene_sun, BLUE_SUNTYPE_COUNT> load_suns_file( const solosnake::filepath& sunspath,
                                                              solosnake::itextureloader& );
}
#endif
