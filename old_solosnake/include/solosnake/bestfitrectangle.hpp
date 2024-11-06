#ifndef solosnake_bestfitrectangle_hpp
#define solosnake_bestfitrectangle_hpp

#include <cstddef>
#include <vector>
#include "solosnake/position.hpp"

namespace solosnake
{
namespace bestfit
{
    class rectangle
    {
    public:
        rectangle(unsigned int w, unsigned int h);

        unsigned int width() const
        {
            return width_;
        }

        unsigned int height() const
        {
            return height_;
        }

        rectangle rotated() const;

    private:
        unsigned int width_;
        unsigned int height_;
    };

    class placedrect
    {
    public:
        placedrect(size_t srcIndex,
                   rectangle Rect,
                   unsigned int cornerX,
                   unsigned int cornerY,
                   bool rotate);

        position2d<unsigned int> position() const;
        bool is_rotated() const;
        unsigned int leftmost() const;
        unsigned int topmost() const;
        unsigned int rightmost() const;
        unsigned int bottommost() const;
        unsigned int width() const;
        unsigned int height() const;
        size_t index() const;

    private:
        size_t index_;
        rectangle rect_;
        position2d<unsigned int> xy_;
        bool wasRotated_;
    };

    //! Returns the bounding rectangle for the placedrect array, assuming
    //! that the location (0,0) is used, e.g. does not take into account
    //! an array where there is some offset from 0,0 in x or y. Assumes
    //! at least one placed rectangle is flush with the origin.
    rectangle bounding_rectangle(const std::vector<placedrect>&);

    //! Returns an array of 'placed' rectangles, which describes how to
    //! arrange the incoming rectangles so as to minimise the bounding volume
    //! of the collection.
    std::vector<placedrect> best_fit_rectangle_containing(const std::vector<rectangle>&);

    //! Intended for testing, saves the array as a bitmap, empty areas are
    // black.
    void save_bestfit_as_bitmap(const char*, const std::vector<solosnake::bestfit::placedrect>&);
}
}

#endif
