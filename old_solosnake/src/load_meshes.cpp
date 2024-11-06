#include <cassert>
#include <fstream>
#include "solosnake/filepath.hpp"
#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/img.hpp"
#include "solosnake/image.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
#define SS_MESHHEADER_MARKER (3238)

    static_assert( sizeof( string::value_type ) == sizeof( char ),
                   "std::string::value_type is size other than sizeof char" );

    namespace
    {
        template <typename T> inline
        void write( std::ofstream& f, const T& t )
        {
            f.write( reinterpret_cast<const char*>( &t ), sizeof( t ) );
        }

        template <typename T> inline
        void write( std::ofstream& f, const vector<T>& t )
        {
            write( f, uint64_t( t.size() ) );
            f.write( reinterpret_cast<const char*>( t.data() ), t.size() * sizeof( T ) );
        }

        template <>
        void write( std::ofstream& f, const string& t )
        {
            write( f, uint64_t( t.length() ) );

            for( size_t i = 0; i < t.length(); ++i )
            {
                write( f, t[i] );
            }
        }

        template <>
        void write(std::ofstream& f, const meshdata& t )
        {
            write( f, t.name );
            write( f, t.diffAndSpecName );
            write( f, t.normalsAndEmissName );
            write( f, t.indices );
            write( f, t.vertices );
            write( f, t.uvs );
            write( f, t.tangents );
            write( f, t.smoothednormals );
        }

        template <typename T> inline
        void read(std::ifstream& f, T& t )
        {
            f.read( reinterpret_cast<char*>( &t ), sizeof( t ) );
        }

        template <typename T> inline
        void read( std::ifstream& f, vector<T>& t )
        {
            vector<T> v;

            uint64_t n = 0u;
            read( f, n );

            if( n )
            {
                v.resize( n );
                f.read( reinterpret_cast<char*>( v.data() ), sizeof( T ) * n );
            }

            t.swap( v );
        }

        template <> inline
        void read(std::ifstream& f, string& t )
        {
            string s;

            uint64_t n = 0u;
            read( f, n );

            if( n )
            {
                s.resize( n );
                for( size_t i = 0; i < n; ++i )
                {
                    string::value_type c = string::value_type();
                    read( f, c );
                    s[i] = c;
                }
            }

            t.swap( s );
        }

        template <>
        void read( std::ifstream& f, meshdata& t )
        {
            read( f, t.name );
            read( f, t.diffAndSpecName );
            read( f, t.normalsAndEmissName );
            read( f, t.indices );
            read( f, t.vertices );
            read( f, t.uvs );
            read( f, t.tangents );
            read( f, t.smoothednormals );
        }

        template <typename T>
        uint64_t datasize( const vector<T>& v )
        {
            return sizeof( uint64_t ) + ( v.size() * sizeof( T ) );
        }

        uint64_t datasize( const string& txt )
        {
            return sizeof( uint64_t ) + txt.length() * sizeof( string::value_type );
        }

        uint64_t datasize( const meshdata& d )
        {
            uint64_t s = 0u;

            s += datasize( d.name );
            s += datasize( d.diffAndSpecName );
            s += datasize( d.normalsAndEmissName );
            s += datasize( d.indices );
            s += datasize( d.vertices );
            s += datasize( d.uvs );
            s += datasize( d.tangents );
            s += datasize( d.smoothednormals );

            return s;
        }

        //! Loads image and checks it is in RGBA format.
        image load_rgba_image( const string& file, solosnake::ifilefinder& texfinder )
        {
            image img( texfinder.get_file( file ).string(), false );

            // Check all images are correct format:
            if( img.format() != image::format_bgra )
            {
                ss_err( "File is not in required RGBA format.", file );
                ss_throw( "Texture is not in required RGBA format." );
            }

            return img;
        }

        class meshinterface
            : public imeshgeometry
            , public imeshtextureinfo
        {
        public:

            meshinterface( meshdata&&, const shared_ptr<ifilefinder>& );
            virtual ~meshinterface() SS_NOEXCEPT;
            std::string  name() const override;
            IndexMode    index_mode() const override;
            unsigned int vertex_count() const override;
            unsigned int index_count() const override;
            const std::uint16_t* indices() const override;
            const float* vertices() const override;
            iimg_ptr     tex_diffuse_and_spec() const override;
            iimg_ptr     tex_normals_and_emiss() const override;
            const float* tangents() const override;
            const float* smoothed_normals() const override;
            const float* texture_coords() const override;
            unsigned int texture_coords_count() const override;

        public:
            shared_ptr<ifilefinder> texfinder_;
            mutable iimg_ptr        tex_diffuse_and_spec_;
            mutable iimg_ptr        tex_normals_and_emiss_;
            meshdata                mesh_;
        };

        class meshdatamesh : public solosnake::imesh
        {
        public:
            meshdatamesh( meshdata&&, const shared_ptr<ifilefinder>& );
            virtual ~meshdatamesh() SS_NOEXCEPT;
            std::string get_mesh_name() const override;
            std::shared_ptr<imeshgeometry> get_meshgeometry() override;
            std::shared_ptr<imeshtextureinfo> get_meshgeometrytexinfo() override;

        private:
            shared_ptr<meshinterface> mesh_;
        };

        //-------------------------------------------------------------------------

        meshinterface::meshinterface( meshdata&& m,
                                      const shared_ptr<ifilefinder>& texfinder )
            : texfinder_( texfinder )
            , mesh_( move( m ) )
        {
            if( ( mesh_.vertices.size() % 3u ) != 0u )
            {
                ss_throw( "meshdata vertex buffer has non multiple of 3 contents." );
            }

            if( ( mesh_.uvs.size() % 2u ) != 0u )
            {
                ss_throw( "meshdata UV buffer has non multiple of 2 contents." );
            }

            if( ( mesh_.tangents.size() % 3u ) != 0u )
            {
                ss_throw( "meshdata tangents buffer has non multiple of 3 contents." );
            }

            if( ( mesh_.smoothednormals.size() % 3u ) != 0u )
            {
                ss_throw( "meshdata smoothed normals buffer has non multiple of 3 "
                          "contents." );
            }
        }

        meshinterface::~meshinterface() SS_NOEXCEPT
        {
        }

        std::string meshinterface::name() const
        {
            return mesh_.name;
        }

        imeshgeometry::IndexMode meshinterface::index_mode() const
        {
            return imeshgeometry::TrianglesMode;
        }

        unsigned int meshinterface::vertex_count() const
        {
            return static_cast<unsigned int>( mesh_.vertices.size() / 3u );
        }

        unsigned int meshinterface::index_count() const
        {
            return static_cast<unsigned int>( mesh_.indices.size() );
        }

        const std::uint16_t* meshinterface::indices() const
        {
            return mesh_.indices.data();
        }

        const float* meshinterface::vertices() const
        {
            return mesh_.vertices.data();
        }

        iimg_ptr meshinterface::tex_diffuse_and_spec() const
        {
            if( nullptr == tex_diffuse_and_spec_.get() )
            {
                tex_diffuse_and_spec_ =
                    make_shared<img>( load_rgba_image( mesh_.diffAndSpecName, *texfinder_ ) );
            }

            return tex_diffuse_and_spec_;
        }

        iimg_ptr meshinterface::tex_normals_and_emiss() const
        {
            if( nullptr == tex_normals_and_emiss_.get() )
            {
                tex_normals_and_emiss_ =
                    make_shared<img>( load_rgba_image( mesh_.normalsAndEmissName, *texfinder_ ) );
            }

            return tex_normals_and_emiss_;
        }

        const float* meshinterface::tangents() const
        {
            return mesh_.tangents.data();
        }

        const float* meshinterface::smoothed_normals() const
        {
            return mesh_.smoothednormals.data();
        }

        const float* meshinterface::texture_coords() const
        {
            return mesh_.uvs.data();
        }

        unsigned int meshinterface::texture_coords_count() const
        {
            return this->vertex_count();
        }

        //-------------------------------------------------------------------------

        meshdatamesh::meshdatamesh( meshdata&& m, const shared_ptr<ifilefinder>& texfinder )
            : mesh_( make_shared<meshinterface>( move( m ), texfinder ) )
        {
        }

        meshdatamesh::~meshdatamesh() SS_NOEXCEPT
        {
        }

        std::string meshdatamesh::get_mesh_name() const
        {
            return mesh_->name();
        }

        std::shared_ptr<imeshgeometry> meshdatamesh::get_meshgeometry()
        {
            return static_pointer_cast<imeshgeometry>( mesh_ );
        }

        std::shared_ptr<imeshtextureinfo> meshdatamesh::get_meshgeometrytexinfo()
        {
            return static_pointer_cast<imeshtextureinfo>( mesh_ );
        }

    } // anonymous namespace

    shared_ptr<imesh> load_mesh( const string&,
                                 const filepath& filename,
                                 const shared_ptr<ifilefinder>& )
    {
        shared_ptr<imesh> result;

        if( false == std::filesystem::is_regular_file( filename ) )
        {
            ss_throw( "File specified for load_mesh does not exist." );
        }

        ss_throw( "NOT IMPLEMENTED" );
    }

    void save_mesh_datas( const filepath& url,
                          const vector<meshdata>& meshes )
    {
        std::ofstream meshfile( url, ios::binary | ios::out | ios::trunc );

        if( meshfile.is_open() )
        {
            write( meshfile, uint64_t( SS_MESHHEADER_MARKER ) );
            write( meshfile, uint64_t( meshes.size() ) );

            uint64_t offset = ( meshes.size() + 2 ) * sizeof( uint64_t );

            vector<uint64_t> offsets( meshes.size(), 0u );

            for( size_t i = 0; i < meshes.size(); ++i )
            {
                write( meshfile, offset );
                offsets[i] = offset;
                offset += datasize( meshes[i] );
            }

            for( size_t i = 0; i < meshes.size(); ++i )
            {
                assert( static_cast<uint64_t>( meshfile.tellp() ) == offsets[i] );
                write( meshfile, meshes[i] );
            }

            if( false == meshfile.good() )
            {
                ss_err( "Error writing  ", url.string() );
                ss_throw( "Error while writing mesh file." );
            }

            meshfile.close();
        }
        else
        {
            ss_err( "Cannot create or open ", url.string() );
            ss_throw( "Unable to create mesh file." );
        }
    }

    //! Opens and reads the named binary file and returns its contents as an
    //! array of meshes. Internally these meshes will be meshdata structs.
    vector<shared_ptr<imesh>> load_mesh_datas( const filepath& url,
                                               const shared_ptr<ifilefinder>& texfinder )
    {
        std::ifstream meshfile( url.string(), ios::binary | ios::in );

        if( false == meshfile.is_open() )
        {
            ss_err( "Unable to read mesh file ", url.string() );
            ss_throw( "Unable to open mesh file for reading." );
        }

        vector<shared_ptr<imesh>> meshes;

        uint64_t marker = 0u;
        uint64_t meshcount = 0u;

        read( meshfile, marker );

        if( marker != SS_MESHHEADER_MARKER )
        {
            ss_throw( "Invalid mesh file." );
        }

        read( meshfile, meshcount );

        meshes.resize( meshcount );
        vector<uint64_t> offsets( meshcount, 0u );
        meshfile.read( reinterpret_cast<char*>( offsets.data() ), sizeof( uint64_t ) * meshcount );

        for( uint64_t i = 0; i < meshcount; ++i )
        {
            meshdata d;
            read( meshfile, d );
            meshes[i] = make_shared<meshdatamesh>( move( d ), texfinder );
        }

        return meshes;
    }
}
