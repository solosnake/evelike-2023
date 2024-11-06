#ifndef solosnake_renderer_cache_hpp
#define solosnake_renderer_cache_hpp

#include <map>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include "solosnake/filepath.hpp"
#include "solosnake/deferred_renderer_fwd.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/fontcache.hpp"
#include "solosnake/itextureloader.hpp"
#include "solosnake/modelnode.hpp"
#include "solosnake/texturehandle.hpp"

namespace solosnake
{
    class font;
    class imesh_cache;
    class iimgsrc;
    class iimg_cache;
    class iskybox_cache;
    class ifilefinder;
    class modeldescription;
    class modelnodedescription;
    class submeshdescription;

    //! Intended for use with a known renderer, this should be used to sit
    //! between a renderer and the user to save known handles. As it will store
    //! the renderer it can also be used as a placeholder to put the renderer.
    //! This object performs the caching of the handles, ensuring we always
    //! get the same renderer handle per named mesh or image.
    class renderer_cache : public itextureloader, public fontcache
    {
    public:

        renderer_cache(
            std::shared_ptr<ifilefinder> modelspaths,
            std::shared_ptr<ifilefinder> fontspaths,
            std::shared_ptr<imesh_cache>,
            std::shared_ptr<iimg_cache>,
            std::shared_ptr<iskybox_cache>,
            std::shared_ptr<deferred_renderer> );

        virtual ~renderer_cache();

        void load_models_file( const std::string& );

        void load_meshes_file( const std::string& );

        void load_skybox_file( const std::string& );

        texturehandle_t get_texture( const std::string& ) override;

        meshhandle_t get_mesh( const std::string& );

        std::shared_ptr<iimgsrc> get_img_src( const std::string& );

        std::shared_ptr<cubemap> get_skybox( const std::string& );

        std::shared_ptr<deferred_renderer> get_renderer_ptr();

        deferred_renderer& get_renderer();

        void compact_cache();

        modelnode instance_model( const std::string& modelname );

        deferred_renderer* operator->();

    private:

        texturehandle_t get_font_texture( const std::string& ) override;

        modelnode construct_model( const modeldescription& );

        modelnode recursively_construct_model( const modelnodedescription& );

    private:

        std::shared_ptr<deferred_renderer>      renderer_;
        std::shared_ptr<imesh_cache>            meshes_;
        std::shared_ptr<iimg_cache>             images_;
        std::shared_ptr<iskybox_cache>          skyboxes_;
        std::map<std::string, meshhandle_t>     knownmeshes_;
        std::map<std::string, texturehandle_t>  knowntextures_;
        std::map<std::string, modelnode>        knownmodels_;
        std::shared_ptr<ifilefinder>            modelspaths_;
        std::shared_ptr<ifilefinder>            fontspaths_;
        std::shared_ptr<fontcache>              font_cache_;
        std::vector<std::string>                loaded_modelsfiles_;
    };

    //---------------------------------------------------------------------------------------------

    inline std::shared_ptr<deferred_renderer> renderer_cache::get_renderer_ptr()
    {
        return renderer_;
    }

    inline deferred_renderer& renderer_cache::get_renderer()
    {
        return *renderer_;
    }

    inline deferred_renderer* renderer_cache::operator->()
    {
        return renderer_.get();
    }
}

#endif
