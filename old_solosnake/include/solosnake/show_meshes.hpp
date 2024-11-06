#ifndef solosnake_show_meshes_hpp
#define solosnake_show_meshes_hpp

namespace solosnake
{
    //! Creates and shows a window and tries to load mesh files named in the
    //! C style array of mesh file names. Control returns from this function
    //! when the user closes the window.
    //! This call is not thread safe and should not be called from more than
    //! one thread.
    //! @param separation The distance models are separated by when rendering
    //!                   multiple copies.
    int show_meshes( int meshCount, const char* const* meshFiles, const float separation = 1.0f );
}

#endif
