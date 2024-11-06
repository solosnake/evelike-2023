#include "solosnake/deferred_renderer_types.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    //-------------------------------------------------------------------------

    //! Returns true if and only if contents are identical.
    bool imeshgeometry::operator==( const imeshgeometry& rhs ) const
    {
        return index_mode() == rhs.index_mode() 
               && name() == rhs.name()
               && index_count() == rhs.index_count() 
               && vertex_count() == rhs.vertex_count()
               && 0 == memcmp( vertices(), rhs.vertices(), sizeof( float ) * vertex_count() )
               && 0 == memcmp( indices(), rhs.indices(), sizeof( std::uint16_t ) * index_count() );
    }

    //! Returns true if contents are not identical.
    bool imeshgeometry::operator!=( const imeshgeometry& rhs ) const
    {
        return false == ( *this == rhs );
    }

    //! Checks that all indices are valid.
    bool imeshgeometry::validate() const
    {
        if( index_count() > 0 )
        {
            switch( index_mode() )
            {
                case TrianglesMode:
                {
                    const size_t maxindex = vertex_count() - 1;
                    for( size_t i = 0; i < index_count(); ++i )
                    {
                        if( indices()[i] > maxindex )
                        {
                            return false;
                        }
                    }
                }
                break;

                case TriangleStripsMode:
                {
                    const size_t maxindex = vertex_count() - 1;
                    for( size_t i = 0; i < index_count(); ++i )
                    {
                        if( indices()[i] > maxindex && indices()[i] != StripRestartMarker )
                        {
                            return false;
                        }
                    }
                }
                break;

                default:
                    ss_throw( "Unhandled IndexMode." );
            }
        }

        return true;
    }

    //-------------------------------------------------------------------------

    iimgsrc::~iimgsrc() SS_NOEXCEPT
    {
    }

    //-------------------------------------------------------------------------

    iimg::~iimg() SS_NOEXCEPT
    {
    }

    bool iimg::operator==( const iimg& rhs ) const
    {
        return img_width() == rhs.img_width() && img_height() == rhs.img_height()
               && ( 0
                    == memcmp( img_bgra_pixels(), rhs.img_bgra_pixels(), 4 * img_width() * img_height() ) );
    }

    bool iimg::operator!=( const iimg& rhs ) const
    {
        return false == ( *this == rhs );
    }
    //-------------------------------------------------------------------------

    iimgBGR::~iimgBGR() SS_NOEXCEPT
    {
    }

    bool iimgBGR::operator==( const iimgBGR& rhs ) const
    {
        return img_width() == rhs.img_width() && img_height() == rhs.img_height()
               && ( 0 == memcmp( img_bgr_pixels(), rhs.img_bgr_pixels(), 3 * img_width() * img_height() ) );
    }

    bool iimgBGR::operator!=( const iimgBGR& rhs ) const
    {
        return false == ( *this == rhs );
    }

    //-------------------------------------------------------------------------

    inline bool same_image( iimg_ptr a, iimg_ptr b )
    {
        return ( a == b ) || ( ( a && b && ( *a == *b ) ) );
    }

    imeshtextureinfo::~imeshtextureinfo() SS_NOEXCEPT
    {
    }

    bool imeshtextureinfo::operator==( const imeshtextureinfo& rhs ) const
    {
        return name() == rhs.name() && same_image( tex_diffuse_and_spec(), rhs.tex_diffuse_and_spec() )
               && same_image( tex_normals_and_emiss(), rhs.tex_diffuse_and_spec() );
    }

    bool imeshtextureinfo::operator!=( const imeshtextureinfo& rhs ) const
    {
        return false == ( *this == rhs );
    }

    //-------------------------------------------------------------------------

    imeshtexturecoords::~imeshtexturecoords() SS_NOEXCEPT
    {
    }

    //-------------------------------------------------------------------------

    dynbuffer::dynbuffer( const unsigned int nv, const unsigned int ni )
        : verts_( nullptr )
        , coords_( nullptr )
        , colours_( nullptr )
        , indices_( nullptr )
        , vcount_( 0 )
        , icount_( 0 )
    {
        assert( nv > 0u );
        assert( ni > 0u );
        unique_ptr<dynbuffervertex[]> v( new dynbuffervertex[nv] );
        unique_ptr<dynbufferuv[]> u( new dynbufferuv[nv] );
        unique_ptr<dynbufferRGBA[]> c( new dynbufferRGBA[nv] );
        unique_ptr<unsigned short[]> i( new unsigned short[ni] );
        verts_   = v.release();
        coords_  = u.release();
        colours_ = c.release();
        indices_ = i.release();
        vcount_  = nv;
        icount_  = ni;
    }

    dynbuffer::~dynbuffer() SS_NOEXCEPT
    {
        assert( icount_ );
        assert( vcount_ );

        delete [] verts_;
        verts_ = nullptr;

        delete [] colours_;
        colours_ = nullptr;

        delete [] coords_;
        coords_ = nullptr;

        delete [] indices_;
        indices_ = nullptr;

        vcount_ = 0;
        icount_ = 0;
    }

    dynbufferRGBA lerp( dynbufferRGBA rgb0, dynbufferRGBA rgb1, float f )
    {
        float r = static_cast<float>( rgb0.red );
        float g = static_cast<float>( rgb0.green );
        float b = static_cast<float>( rgb0.blue );
        float a = static_cast<float>( rgb0.alpha );

        float diffr = static_cast<float>( rgb1.red )   - r;
        float diffg = static_cast<float>( rgb1.green ) - g;
        float diffb = static_cast<float>( rgb1.blue )  - b;
        float diffa = static_cast<float>( rgb1.alpha ) - a;

        dynbufferRGBA c;

        c.red   = static_cast<unsigned char>( r + diffr * f );
        c.green = static_cast<unsigned char>( g + diffg * f );
        c.blue  = static_cast<unsigned char>( b + diffb * f );
        c.alpha = static_cast<unsigned char>( a + diffa * f );

        return c;
    }

}
