#ifndef solosnake_foreachtriangle_hpp
#define solosnake_foreachtriangle_hpp

#include <functional>

namespace solosnake
{
    class imeshgeometry;
    class imeshtexturecoords;

    //! Helper class for iteration over the triangles contained in a model.
    class for_each_triangle
    {
    public:

        struct vertex
        {
            float x;
            float y;
            float z;
            float u;
            float v;
        };

        struct triangle
        {
            vertex verts[3];
            unsigned short indices[3];
            unsigned short id; // 0 ... n triangles.
        };

        typedef std::function<void( const imeshgeometry&, const triangle& )> callback;

        static void iterate( const imeshgeometry&, const imeshtexturecoords&, callback );
    };
}

#endif
