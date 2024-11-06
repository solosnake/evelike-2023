#ifndef solosnake_texquad_hpp
#define solosnake_texquad_hpp

namespace solosnake
{
    //! A very basic unconstrained quad struct, used for specifying locations
    //! on texture maps and on screen rectangles.
    struct texquad
    {
        texquad()
            : x0(0.0f), y0(0.0f), x1(1.0f), y1(1.0f) 
        {
        }

        texquad(const float u0, const float v0, const float u1, const float v1)
            : x0(u0), y0(v0), x1(u1), y1(v1) 
        {
        }

        float x0;
        float y0;
        float x1;
        float y1;
    };

    inline bool operator==( const texquad& lhs, const texquad& rhs )
    {
        return lhs.x0 == rhs.x0 && lhs.y0 == rhs.y0 && lhs.x1 == rhs.x1 && lhs.y1 == rhs.y1;
    }

    inline bool operator!=( const texquad& lhs, const texquad& rhs )
    {
        return lhs.x0 != rhs.x0 || lhs.y0 != rhs.y0 || lhs.x1 != rhs.x1 || lhs.y1 != rhs.y1;
    }
}

#endif
