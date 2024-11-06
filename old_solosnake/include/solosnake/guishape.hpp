#ifndef solosnake_guishape_hpp
#define solosnake_guishape_hpp

#include <vector>
#include "solosnake/ishape.hpp"
#include "solosnake/point.hpp"

namespace solosnake
{
    //! A 2D shape composed of n 2D triangles.
    class guishape : public ishape
    {
    public:

        void add_triangle( const point2d& a, const point2d& b, const point2d& c );

        bool is_xy_inside_shape( float x, float y ) const override;

    private:
      
        struct tri
        {
            point2d corners_[3];
        };

        std::vector<tri> triangles_;
    };
}

#endif
