#include "solosnake/colourtriangle.hpp"

namespace solosnake
{
    colourtriangle2d::colourtriangle2d( const point2_t v0,
                                        const bgra& c0,
                                        const point2_t v1,
                                        const bgra& c1,
                                        const point2_t v2,
                                        const bgra& c2 )
        : barytri_( v0, v1, v2 ), c0_( c0 ), c1_( c1 ), c2_( c2 )
    {
    }

    colourtriangle2d::colourtriangle2d( float x0,
                                        float y0,
                                        const bgra& c0,
                                        float x1,
                                        float y1,
                                        const bgra& c1,
                                        float x2,
                                        float y2,
                                        const bgra& c2 )
        : barytri_( x0, y0, x1, y1, x2, y2 ), c0_( c0 ), c1_( c1 ), c2_( c2 )
    {
    }

    bgra colourtriangle2d::get_barycentric_colour( float x, float y ) const
    {
        point3_t barycoord;
        barytri_.get_barycentric_coords( x, y, barycoord );

        return bgra(
                   static_cast<std::uint8_t>( barycoord[0] * c0_.blue()  + barycoord[1] * c1_.blue()  + barycoord[2] * c2_.blue() ),
                   static_cast<std::uint8_t>( barycoord[0] * c0_.green() + barycoord[1] * c1_.green() + barycoord[2] * c2_.green() ),
                   static_cast<std::uint8_t>( barycoord[0] * c0_.red()   + barycoord[1] * c1_.red()   + barycoord[2] * c2_.red() ),
                   static_cast<std::uint8_t>( barycoord[0] * c0_.alpha() + barycoord[1] * c1_.alpha() + barycoord[2] * c2_.alpha() ) );
    }
}
