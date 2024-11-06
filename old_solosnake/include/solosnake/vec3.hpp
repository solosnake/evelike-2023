#ifndef solosnake_vec3_hpp
#define solosnake_vec3_hpp

namespace solosnake
{
    //! Class to allow emulation of OpenGL GLSL types.
    struct vec3
    {
        vec3() : x( 0.0f ), y( 0.0f ), z( 0.0f )
        {
        }

        vec3( const float* p ) : x( p[0] ), y( p[1] ), z( p[2] )
        {
        }

        vec3( const float a, const float b, const float c ) : x( a ), y( b ), z( c )
        {
        }

        float x, y, z;
    };

    inline vec3 operator/( const vec3& lhs, const float rhs )
    {
        return vec3( lhs.x / rhs, lhs.y / rhs, lhs.z / rhs );
    }

    inline vec3 normalize( const vec3& v )
    {
        const float m = std::sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
        return v / m;
    }

    inline vec3 cross( const vec3& lhs, const vec3& rhs )
    {
        return vec3( lhs.y * rhs.z - lhs.z * rhs.y,
                     lhs.z * rhs.x - lhs.x * rhs.z,
                     lhs.x * rhs.y - lhs.y * rhs.x );
    }

    inline float dot( const vec3& lhs, const vec3& rhs )
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
}

#endif
