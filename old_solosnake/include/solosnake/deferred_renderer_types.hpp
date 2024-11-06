#ifndef solosnake_deferred_renderer_types_hpp
#define solosnake_deferred_renderer_types_hpp

#include <vector>
#include <memory>
#include <string>
#include "solosnake/matrix3d.hpp"
#include "solosnake/deferred_renderer_meshinstancedata.hpp"
#include "solosnake/noexcept.hpp"

#define SS_TEXCOORDS_ARRAYNAME "texture_coords"
#define SS_TEXTANGENTS_ARRAYNAME "texture_tangents"
#define SS_SMOOTHEDNORMALS_ARRAYNAME "smoothed_normals"
#define SS_DIFF_BGR_SPEC_A "diffuse_bgr_spec_a"
#define SS_NORMALS_BG_EMISS_A "normals_bg_emiss_ra"

//! @file
//! This contains the types and interfaces used by the deferred_renderer.
//! You can implement your own versions of most of these interfaces to
//! interact with the deferred_renderer.

namespace solosnake
{
    //! \brief Simple BGRA texture class.
    //! \details A contiguous 4-byte per pixel W x H rectangle of BGRA pixels.
    class texture
    {
    public:
        texture();

        texture( texture&& );

        texture& operator=( texture && );

        texture( size_t width, size_t height, const unsigned char* bgraData );

        const unsigned char* bgra_bytes() const;

        size_t width() const;

        size_t height() const;

    private:
        std::vector<unsigned char> bgra_;
        size_t width_;
    };

    //! @brief A single mesh of a mesh.
    //! @details A mesh is a set of 3D vertices (x,y,z) and a set of 2D texture
    //! coordinates (s,t), a set of unsigned short indices which are either
    //! triangles or triangle strips. When using triangle strips, strip restart
    //! points should be indicated using the StripRestartMarker value.
    //! A mesh is uniquely identified by its name. It also knows its parent name
    //! so that several meshes can be combined when drawn to form a conceptual
    //! combined model. The matching texture coordinates for a mesh should come
    //! from an imeshtextureinfo with the same name and parent name.
    class imeshgeometry
    {
    public:

        enum
        {
            StripRestartMarker = 0xFFFF
        };

        enum IndexMode
        {
            TrianglesMode,
            TriangleStripsMode
        };

        virtual ~imeshgeometry() SS_NOEXCEPT {};

        virtual std::string name() const = 0;

        virtual IndexMode index_mode() const = 0;

        virtual unsigned int vertex_count() const = 0;

        virtual unsigned int index_count() const = 0;

        virtual const std::uint16_t* indices() const = 0;

        virtual const float* vertices() const = 0;

        bool validate() const;

        bool operator==( const imeshgeometry& ) const;

        bool operator!=( const imeshgeometry& ) const;
    };

    //! @brief A class for holding just the 2D texture coords, independently of
    //! the textures themselves. This can be paired with a meshgeometry, in
    //! which case the meshgeometry indices will refer to [u,v] pairs within
    //! this object, and the number of vertices should match this objects number
    //! of texture coord pairs.
    class imeshtexturecoords
    {
    public:
        virtual ~imeshtexturecoords() SS_NOEXCEPT;

        //! Returns a contiguous array of [u,v] coordinates.
        virtual const float* texture_coords() const = 0;

        //! This is the number of 2D coordinates: e.g. the number of [u,v] pairs.
        virtual unsigned int texture_coords_count() const = 0;
    };

    //! @brief A basic BGRA image class. Assumes tightly packed bytes.
    class iimg
    {
    public:
        virtual ~iimg() SS_NOEXCEPT;

        virtual unsigned int img_width() const = 0;

        virtual unsigned int img_height() const = 0;

        virtual const unsigned char* img_bgra_pixels() const = 0;

        bool operator==( const iimg& ) const;

        bool operator!=( const iimg& ) const;
    };

    //! @brief A BGR image class. Assumes tightly packed bytes.
    class iimgBGR
    {
    public:
        virtual ~iimgBGR() SS_NOEXCEPT;

        virtual unsigned int img_width() const = 0;

        virtual unsigned int img_height() const = 0;

        virtual const unsigned char* img_bgr_pixels() const = 0;

        bool operator==( const iimgBGR& ) const;

        bool operator!=( const iimgBGR& ) const;
    };

    typedef std::shared_ptr<iimg>       iimg_ptr;
    typedef std::shared_ptr<iimgBGR>    iimgBGR_ptr;

    //! @brief A factory for returning a given image.
    class iimgsrc
    {
    public:
        virtual ~iimgsrc() SS_NOEXCEPT;

        virtual std::string get_image_name() const = 0;

        virtual iimg_ptr get_image() = 0;
    };

    //! @brief A class to hold the memory expensive and less frequently referenced
    // texture.
    //! specific data elements of a mesh. Note that mesh texture infos do not
    //! have parent model names. This is because they are intended to be paired
    //! with their mesh first, and the mesh knows the parent model name.
    class imeshtextureinfo : public imeshtexturecoords
    {
    public:
        virtual ~imeshtextureinfo() SS_NOEXCEPT;

        virtual std::string name() const = 0;

        virtual iimg_ptr tex_diffuse_and_spec() const = 0;

        virtual iimg_ptr tex_normals_and_emiss() const = 0;

        //! Returns a pointer to a buffer of tangents (each tangent is 3 floats).
        virtual const float* tangents() const = 0;

        //! Returns a pointer to a buffer of smoothed face normals (each is 3
        // floats).
        virtual const float* smoothed_normals() const = 0;

        bool operator==( const imeshtextureinfo& ) const;

        bool operator!=( const imeshtextureinfo& ) const;
    };

    //! @brief A factory interface for returning a particular mesh.
    //! @details This allows for lazy loading or reloading of a mesh upon request.
    //! This is the complete data for a single mesh (geometry + texture). A
    //! complete 'model' may be composed of several meshes.
    class imesh
    {
    public:
        virtual ~imesh() SS_NOEXCEPT;
        virtual std::string get_mesh_name() const = 0;
        virtual std::shared_ptr<imeshgeometry> get_meshgeometry() = 0;
        virtual std::shared_ptr<imeshtextureinfo> get_meshgeometrytexinfo() = 0;
    };

    struct dynbuffervertex
    {
        float x, y, z;
    };

    struct dynbufferuv
    {
        float u, v;
    };

    struct dynbufferRGBA
    {
        unsigned char red, green, blue, alpha;
    };

    dynbufferRGBA lerp( dynbufferRGBA rgb0, dynbufferRGBA rgb1, float f );

    //!
    class dynbuffer
    {
    public:

        static const unsigned short StripRestartMarker = 0xFFFF;

        const dynbuffervertex* vertices() const       { return verts_; }
                                                      
        const dynbufferRGBA* colours() const          { return colours_; }
                                                      
        const unsigned short* indices() const         { return indices_; }
                                                      
        const dynbufferuv* texcoords() const          { return coords_; }
                                                      
        dynbuffervertex* vertices()                   { return verts_;  }
                                                      
        dynbufferRGBA* colours()                      { return colours_; }
                                                      
        unsigned short* indices()                     { return indices_; }
                                                      
        dynbufferuv* texcoords()                      { return coords_; }

        unsigned int colours_count() const            { return vcount_; }

        unsigned int vertex_count() const             { return vcount_; }

        unsigned int index_count() const              { return icount_; }

        unsigned int texcoord_count() const           { return vcount_; }

        dynbufferRGBA& colour( unsigned int i )       { return colours_[i]; }
                                                      
        dynbuffervertex& vertex( unsigned int i )     { return verts_[i]; }
                                                      
        unsigned short& indice( unsigned int i )      { return indices_[i]; }
                                                      
        dynbufferuv& texcoord( unsigned int i )       { return coords_[i]; }

    protected:

        dynbuffer( unsigned int nv, unsigned int ni );

        virtual ~dynbuffer() SS_NOEXCEPT;

    private:

        dynbuffer( const dynbuffer& );
        dynbuffer& operator=( const dynbuffer& );

    private:

        dynbuffervertex*    verts_;
        dynbufferuv*        coords_;
        dynbufferRGBA*      colours_;
        unsigned short*     indices_;
        unsigned int        vcount_;
        unsigned int        icount_;
    };

    //! Sfx are primitives which are not lit by the scene, which do not add
    //! to the z buffer, but which respect the z-buffer and which are also
    //! alpha faded away when they are close to scene polygons. Sfx are
    //! intended to be used to render smoke and explosion or laser effects.
    //! They can be multiplied with a color when being rendered. They are
    //! renderer using 8 channels of information in 2 textures t1 and t2.
    //!
    //! t1.rgb = diffuse color - this color is written to the diffuse color
    //!                          layer of the final scene.
    //! t1.a   = x warp value, used in post effect stage to 'warp' the final
    //!          image. This value is applied as an x offset to the texture lookup.
    //! t2.rgb = emissive color - this color is written to the emissive color
    //!                           layer of the final scene and may be blurred.
    //! t2.a   = y warp value, used in post effect stage to 'warp' the final
    //!          image. This value is applied as an y offset to the texture lookup.
    struct fxtriangle
    {
        struct vertex
        {
            float x;
            float y;
            float z;
            float u;
            float v;
        };

        vertex vert[3];
    };

    static_assert( sizeof( fxtriangle ) == sizeof( float ) * 3 * 5, "fxtriangle is not packed" );

    //! A cubemap is six RGB (no alpha) images in agreed positions relative
    //! to each other which the renderer will (optionally) use as a backdrop
    //! to the scene
    struct cubemap
    {
        enum CubemapFace
        {
            FaceXP,
            FaceXN,
            FaceYP,
            FaceYN,
            FaceZP,
            FaceZN
        };

        iimgBGR_ptr& positive_x()
        {
            return cubemapImages_[0];
        }

        iimgBGR_ptr& negative_x()
        {
            return cubemapImages_[1];
        }

        iimgBGR_ptr& positive_y()
        {
            return cubemapImages_[2];
        }

        iimgBGR_ptr& negative_y()
        {
            return cubemapImages_[3];
        }

        iimgBGR_ptr& positive_z()
        {
            return cubemapImages_[4];
        }

        iimgBGR_ptr& negative_z()
        {
            return cubemapImages_[5];
        }

        const iimgBGR_ptr& operator[]( const size_t n ) const
        {
            return cubemapImages_[n];
        }

        iimgBGR_ptr& operator[]( const size_t n )
        {
            return cubemapImages_[n];
        }

        iimgBGR_ptr cubemapImages_[6];
    };

    struct sun_noise
    {
        sun_noise() : noise_rotation_(), noise_move_x_(), noise_move_y_()
        {
        }

        float           noise_rotation_;
        float           noise_move_x_;
        float           noise_move_y_;
    };

    //! Used when rendering an instance of a sun type.
    struct sun_location_and_radius
    {
        float           world_location_[3];
        float           radius_;
    };

#define SS_DIRTY_VIEWPROJ   (1u)
#define SS_DIRTY_INVIEWPROJ (2u)
#define SS_DIRTY_INVIEW     (4u)
#define SS_DIRTY_INVPROJ    (8u)

    //! Manages the view and projection matrix, minimising the
    //! inverts and multiplications required.
    class viewprojmatrices
    {
        matrix4x4_t             view_;
        matrix4x4_t             proj_;
        mutable matrix4x4_t     viewproj_;
        mutable matrix4x4_t     iviewproj_;
        mutable matrix4x4_t     iview_;
        mutable matrix4x4_t     iproj_;
        float                   zNear_;
        mutable unsigned int    flags_;

    public:

        viewprojmatrices() : zNear_( 0.0f ), flags_( 0 )
        {
            load_identity_4x4( view_ );
            load_identity_4x4( proj_ );
            load_identity_4x4( viewproj_ );
            load_identity_4x4( iviewproj_ );
            load_identity_4x4( iview_ );
            load_identity_4x4( iproj_ );
        }

        void set_view_matrix( const float* unaliased m )
        {
            ASSERT_UNALIASED( m, view_, sizeof( float ) * 16, sizeof( float ) * 16 );
            copy_4x4( m, view_ );
            flags_ |= SS_DIRTY_VIEWPROJ | SS_DIRTY_INVIEWPROJ | SS_DIRTY_INVIEW;
        }

        void set_proj_matrix( const float* unaliased m, float zNear )
        {
            ASSERT_UNALIASED( m, view_, sizeof( float ) * 16, sizeof( float ) * 16 );
            copy_4x4( m, proj_ );
            zNear_ = zNear;
            flags_ |= SS_DIRTY_VIEWPROJ | SS_DIRTY_INVIEWPROJ | SS_DIRTY_INVPROJ;
        }

        const float* view_matrix() const
        {
            return view_;
        }

        const float* proj_matrix() const
        {
            return proj_;
        }

        const float* inv_view_matrix() const
        {
            if( 0 != ( flags_ & SS_DIRTY_INVIEW ) )
            {
                assert( invert_4x4( view_, iview_ ) );
                invert_4x4( view_, iview_ );
                flags_ &= ~SS_DIRTY_INVIEW;
            }

            return iview_;
        }

        const float* inv_proj_matrix() const
        {
            if( 0 != ( flags_ & SS_DIRTY_INVPROJ ) )
            {
                assert( invert_4x4( proj_, iproj_ ) );
                invert_4x4( proj_, iproj_ );
                flags_ &= ~SS_DIRTY_INVPROJ;
            }

            return iproj_;
        }

        const float* viewproj_matrix() const
        {
            if( 0 != ( flags_ & SS_DIRTY_VIEWPROJ ) )
            {
                mul_4x4( view_, proj_, viewproj_ );
                flags_ &= ~SS_DIRTY_VIEWPROJ;
            }

            return viewproj_;
        }

        const float* inv_viewproj_matrix() const
        {
            if( 0 != ( flags_ & SS_DIRTY_INVIEWPROJ ) )
            {
                assert( invert_4x4( viewproj_matrix(), iviewproj_ ) );
                invert_4x4( viewproj_matrix(), iviewproj_ );
                flags_ &= ~SS_DIRTY_INVIEWPROJ;
            }

            return iviewproj_;
        }

        inline float zNear() const
        {
            assert( zNear_ );
            return zNear_;
        }
    };
}

#endif
