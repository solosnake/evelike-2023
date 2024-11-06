#ifndef solosnake_skybox_cache_hpp
#define solosnake_skybox_cache_hpp

#include <map>
#include <memory>
#include "solosnake/iskybox_cache.hpp"

namespace solosnake
{
    class ifilefinder;

    class skybox_cache : public iskybox_cache
    {
    public:

        skybox_cache(
            std::shared_ptr<ifilefinder> skyboxfinder,
            std::shared_ptr<ifilefinder> texfinder );

        ~skybox_cache();

        void load_skybox_file( const std::string& ) override;

        std::shared_ptr<cubemap> get_skybox( const std::string& ) override;

        void compact_cache() override;

    private:

        std::shared_ptr<ifilefinder>                    skyboxfinder_;
        std::shared_ptr<ifilefinder>                    texfinder_;
        std::map<std::string, std::shared_ptr<cubemap>> knownskyboxes_;
    };
}

#endif
