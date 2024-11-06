#ifndef solosnake_load_models_hpp
#define solosnake_load_models_hpp

#include <vector>
#include "solosnake/filepath.hpp"

namespace solosnake
{
    class modeldescription;

    //! Loads zero or more model descriptions from the named Lua file.
    //! A model description specifies the name of the model and the meshes that
    //! compose it. It is a Lua file format.
    //! @code
    //! model {
    //!  name = "solarsails",
    //!  meshfiles = { "solarsails.lua" },
    //!  nodename = "SolarSails",
    //!  mesh = "solarsails00",
    //!  transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    //!  childnodes = {}
    //! }
    //! @endcode
    //!
    std::vector<modeldescription> load_model_descriptions( const filepath& );
}

#endif
