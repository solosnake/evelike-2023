#ifndef solosnake_icosahedron_hpp
#define solosnake_icosahedron_hpp

namespace solosnake
{
    class indexed_triangles;

    //! Returns an icosahedron composed of indexed triangles. The icosahedron
    //! will tightly enclose a sphere of the specified radius.
    indexed_triangles make_icosahedron( float radius );
}

#endif
