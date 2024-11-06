#ifndef solosnake_modeldescription_hpp
#define solosnake_modeldescription_hpp

#include <map>
#include <vector>
#include <string>
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/modelnodedescription.hpp"

namespace solosnake
{
    //! Describes a model by giving it a name listing its child meshes and nodes.
    //! Also lists a set of files to load to ensure the meshes are available.
    class modeldescription
    {
    public:
        //! Constructed with move semantics.
        modeldescription( std::string&& modelname,
                          std::vector<std::string>&& meshdependancies,
                          modelnodedescription&& parentnode );

        const std::string& model_name() const
        {
            return model_name_;
        }

        const modelnodedescription& node() const
        {
            return parentnode_;
        }

        const std::vector<std::string>& mesh_files_required() const
        {
            return dependancies_;
        }

    private:
        std::string model_name_;
        std::vector<std::string> dependancies_;
        modelnodedescription parentnode_;
    };
}

#endif
