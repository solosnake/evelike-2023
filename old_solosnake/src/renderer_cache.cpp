#include "solosnake/renderer_cache.hpp"
#include "solosnake/fontcache.hpp"
#include "solosnake/imesh_cache.hpp"
#include "solosnake/iimg_cache.hpp"
#include "solosnake/iskybox_cache.hpp"
#include "solosnake/load_models.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/modeldescription.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/textureanimationtimeline.hpp"
#include <cassert>
#include <algorithm>

using namespace std;

namespace solosnake
{
    renderer_cache::renderer_cache( shared_ptr<ifilefinder> modelspaths,
                                    shared_ptr<ifilefinder> fontspaths,
                                    shared_ptr<imesh_cache> meshcache,
                                    shared_ptr<iimg_cache> imgcache,
                                    shared_ptr<iskybox_cache> skyboxcache,
                                    std::shared_ptr<deferred_renderer> rndr )
        : fontcache( fontspaths )
        , renderer_( rndr )
        , meshes_( meshcache )
        , images_( imgcache )
        , skyboxes_( skyboxcache )
        , modelspaths_( modelspaths )
        , fontspaths_( fontspaths )
    {
        loaded_modelsfiles_.reserve( 64 );
    }

    renderer_cache::~renderer_cache()
    {
    }

    texturehandle_t renderer_cache::get_font_texture( const std::string& fonttexname )
    {
        return this->get_texture( fonttexname );
    }

    modelnode renderer_cache::instance_model( const string& modelname )
    {
        assert( knownmodels_.count( modelname ) != 0 );
        assert( !modelname.empty() );
        return knownmodels_.at(modelname);
    }

    modelnode renderer_cache::recursively_construct_model( const modelnodedescription& desc )
    {
        modelnode node( desc.node_name(),
                        desc.mesh_name().empty() ? meshhandle_t() : get_mesh( desc.mesh_name() ),
                        desc.instance_data(),
                        desc.child_node_count() );

        for( size_t i = 0; i < desc.child_node_count(); ++i )
        {
            node.add_child_node( recursively_construct_model( desc.child_node( i ) ) );
        }

        return node;
    }

    modelnode renderer_cache::construct_model( const modeldescription& desc )
    {
        return recursively_construct_model( desc.node() );
    }

    void renderer_cache::load_models_file( const string& filename )
    {
        assert( !filename.empty() );

        if( false == binary_search( loaded_modelsfiles_.cbegin(), loaded_modelsfiles_.cend(), filename ) )
        {
            const auto modeldescriptions = load_model_descriptions( modelspaths_->get_file( filename ) );

            // Load all the mesh files named by the model descriptions:
            for( size_t i = 0; i < modeldescriptions.size(); ++i )
            {
                for_each( modeldescriptions[i].mesh_files_required().cbegin(),
                          modeldescriptions[i].mesh_files_required().cend(),
                          [&]( const string & meshfilename )
                {
                    // This is safe to call repeatedly with the same file:
                    load_meshes_file( meshfilename );
                } );
            }

            // Add the new models to the cache of known models:
            for( size_t i = 0; i < modeldescriptions.size(); ++i )
            {
                knownmodels_[modeldescriptions[i].model_name()] = construct_model( modeldescriptions[i] );
            }

            // Remember we have already loaded this file:
            loaded_modelsfiles_.push_back( filename );
            sort( loaded_modelsfiles_.begin(), loaded_modelsfiles_.end() );
        }
    }

    void renderer_cache::load_meshes_file( const string& file )
    {
        meshes_->load_meshes_file( file );
    }

    void renderer_cache::load_skybox_file( const string& file )
    {
        skyboxes_->load_skybox_file( file );
    }

    std::shared_ptr<cubemap> renderer_cache::get_skybox( const std::string& name )
    {
        return skyboxes_->get_skybox( name );
    }

    meshhandle_t renderer_cache::get_mesh( const string& name )
    {
        auto i = knownmeshes_.find( name );

        if( i != knownmeshes_.end() )
        {
            return i->second;
        }

        auto mesh = meshes_->get_imesh( name );
        auto handle = renderer_->load_mesh( mesh );
        knownmeshes_.insert( pair<string, meshhandle_t>( name, handle ) );

        return handle;
    }

    texturehandle_t renderer_cache::get_texture( const string& name )
    {
        auto i = knowntextures_.find( name );

        if( i != knowntextures_.end() )
        {
            return i->second;
        }

        shared_ptr<iimgsrc> src = images_->get_img_src( name );
        auto handle = renderer_->load_texture( src );
        knowntextures_.insert( pair<string, texturehandle_t>( name, handle ) );

        return handle;
    }

    std::shared_ptr<iimgsrc> renderer_cache::get_img_src( const std::string& name )
    {
        return images_->get_img_src( name );
    }

    //! Call this after all loading is completed and rendering is about to begin.
    //! This will attempt to compact the caches and free up memory.
    void renderer_cache::compact_cache()
    {
        meshes_->compact_cache();
        images_->compact_cache();
        skyboxes_->compact_cache();
    }
}