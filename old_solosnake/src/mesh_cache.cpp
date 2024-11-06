#include <cassert>
#include <iostream>
#include "solosnake/load_meshes.hpp"
#include "solosnake/load_models.hpp"
#include "solosnake/mesh_cache.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        class cachedmesh : public imesh
        {
        public:

            cachedmesh( const filepath& file,
                        const shared_ptr<ifilefinder>& texes,
                        const shared_ptr<imesh>& p )
                : name_( p->get_mesh_name() ), file_( file ), texfinder_( texes ), mesh_( p )
            {
            }

            virtual ~cachedmesh() SS_NOEXCEPT
            {
            }

            string get_mesh_name() const override
            {
                return name_;
            }

            shared_ptr<imeshgeometry> get_meshgeometry() override
            {
                if( !mesh_ )
                {
                    reload_mesh();
                }

                return mesh_->get_meshgeometry();
            }

            shared_ptr<imeshtextureinfo> get_meshgeometrytexinfo() override
            {
                if( !mesh_ )
                {
                    reload_mesh();
                }

                return mesh_->get_meshgeometrytexinfo();
            }

            void release_contents()
            {
                mesh_.reset();
            }

            static bool compare_mesh_names( const shared_ptr<cachedmesh>& lhs,
                                            const shared_ptr<cachedmesh>& rhs )
            {
                return lhs->name_ < rhs->name_;
            }

        private:

            void reload_mesh()
            {
                mesh_ = load_mesh( name_, file_, texfinder_ );
            }

        private:

            string                     name_;
            filepath                   file_;
            shared_ptr<ifilefinder>    texfinder_;
            shared_ptr<imesh>          mesh_;
        };
    }

    //! Private hidden implementation class of mesh cache.
    class mesh_cache::mesh_impl
    {
    public:

        mesh_impl( const shared_ptr<ifilefinder>& meshes, const shared_ptr<ifilefinder>& texes );

    private:

        friend class mesh_cache;

        void load_meshes_file( const string& filename );

        shared_ptr<imesh> get_imesh( const string& name );

        void compact_cache();

        size_t size() const;

    private:

        vector<string>                 alreadyloaded_;
        vector<shared_ptr<cachedmesh>> cache_;
        shared_ptr<ifilefinder>        meshfinder_;
        shared_ptr<ifilefinder>        texfinder_;
    };

    //-------------------------------------------------------------------------

    mesh_cache::mesh_impl::mesh_impl( const shared_ptr<ifilefinder>& meshes,
                                      const shared_ptr<ifilefinder>& texes )
        : meshfinder_( meshes )
        , texfinder_( texes )
    {
        assert( meshes && texes );
        cache_.reserve( 64 );
    }

    void mesh_cache::mesh_impl::load_meshes_file( const string& filename )
    {
        if( false == binary_search( alreadyloaded_.cbegin(), alreadyloaded_.cend(), filename ) )
        {
            // Load meshes and infos from Lua:
            auto url = meshfinder_->get_file( filename );
            auto meshes = load_mesh_datas( url, texfinder_ );

            // Put the pairs into the cache as individual objects:
            for( size_t i = 0; i < meshes.size(); ++i )
            {
                cache_.push_back( make_shared<cachedmesh>( url, texfinder_, meshes[i] ) );
            }

            // Sort them by name for faster retrieval.
            sort( cache_.begin(), cache_.end(), &cachedmesh::compare_mesh_names );

            // Remember we have already loaded this file:
            alreadyloaded_.push_back( filename );
            sort( alreadyloaded_.begin(), alreadyloaded_.end() );
        }
    }

    shared_ptr<imesh> mesh_cache::mesh_impl::get_imesh( const string& name )
    {
        auto i = find_if( cache_.cbegin(),
                          cache_.cend(),
                          [&]( const shared_ptr<cachedmesh>& m )
        { return m->get_mesh_name() == name; } );

        if( i == cache_.cend() )
        {
            ss_err( "Requested mesh not found in cache : ", name );
            ss_throw( "Requested mesh not found in cache." );
        }

        return *i;
    }

    void mesh_cache::mesh_impl::compact_cache()
    {
        for( size_t i = 0; i < cache_.size(); ++i )
        {
            cache_[i]->release_contents();
        }
    }

    size_t mesh_cache::mesh_impl::size() const
    {
        return cache_.size();
    }

    //-------------------------------------------------------------------------

    mesh_cache::mesh_cache( const shared_ptr<ifilefinder>& meshes,
                            const shared_ptr<ifilefinder>& texes )
        : impl_( make_shared<mesh_cache::mesh_impl>( meshes, texes ) )
    {
    }

    mesh_cache::~mesh_cache()
    {
    }

    void mesh_cache::load_meshes_file( const string& file )
    {
        impl_->load_meshes_file( file );
    }

    shared_ptr<imesh> mesh_cache::get_imesh( const string& name )
    {
        return impl_->get_imesh( name );
    }

    void mesh_cache::compact_cache()
    {
        impl_->compact_cache();
    }

    size_t mesh_cache::size() const
    {
        return impl_->size();
    }
}
