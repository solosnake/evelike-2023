#ifndef solosnake_make_iimg_hpp
#define solosnake_make_iimg_hpp

#include <memory>

namespace solosnake
{
    class iimg;
    class iimgBGR;
    class image;

    //! Moves a solosnake::image into a newly constructed iimg object.
    std::shared_ptr<iimg> make_iimg( image&& );

    //! Moves a solosnake::image into a newly constructed iimg object.
    std::shared_ptr<iimgBGR> make_iimgBGR( image&& );
}

#endif
