#ifndef solosnake_make_normalmap_hpp
#define solosnake_make_normalmap_hpp

namespace solosnake
{
    class image;
    class imeshgeometry;
    class imeshtexturecoords;

    //! Creates a simple basic normal map using the geometry and texture coords of
    // the
    //! mesh. The map will be width x height, RGBA, and all the alpha values will be
    //! the alpha specified here.
    //! If smooth is true, then vertices that are shared will have their normals
    //! smoothed. If false, then a flat shaded model is produced.
    image make_normalmap( const imeshgeometry&,
                          const imeshtexturecoords&,
                          unsigned int width,
                          unsigned int height,
                          bool smooth,
                          unsigned char alpha );

    //! Overwrites the image @a normalmap with the basic normal map generated
    //! from @a msh and @a tex. This can be used to create a normal map intended
    //! for use by several meshes.
    //! If smooth is true, then vertices that are shared will have their normals
    //! smoothed. If false, then a flat shaded model is produced.
    void add_to_normalmap(
        image&, const imeshgeometry&, const imeshtexturecoords&, bool smooth, unsigned char alpha );
}

#endif
