#include "solosnake/bestfitrectangle.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/colour.hpp"
#include "solosnake/image.hpp"
#include <boost/optional.hpp>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <exception>
#include <memory>
#include <cstdint>

//!
//! \namespace bestfit
//! Code is based on the following article:
//!
// http://www.codeproject.com/Articles/210979/Fast-optimizing-rectangle-packing-algorithm-for-bu
//! however this does not appear to correctly find optimal solutions, but
// instead
//! finds only a decent solution for many cases. It appears unable to find this
//! solution:
//!
//! A 10 x 8 rectangle with no clear "longest" edge, and a hole.
//! \code
//! 0001111111
//! 0001111111
//! 0001111111
//! 0002  4444
//! 0002554444
//! 3333334444
//! 3333334444
//! 3333334444
//! \endcode
//!

using namespace std;

namespace solosnake
{
namespace bestfit
{
    namespace
    {
        typedef shared_ptr<placedrect> placedrect_ptr;

        struct place
        {
            place(unsigned int xx, unsigned int yy) : x(xx), y(yy)
            {
            }

            bool operator==(const place& rhs) const
            {
                return x == rhs.x && y == rhs.y;
            }

            bool operator!=(const place& rhs) const
            {
                return x != rhs.x || y != rhs.y;
            }

            bool operator<(const place& rhs) const
            {
                return x < rhs.x || (x == rhs.x && y < rhs.y);
            }

            unsigned int x;
            unsigned int y;
        };

        class indexedrect : public rectangle
        {
        public:

            indexedrect(const rectangle& r, size_t i) : rectangle(r), index_(i)
            {
            }

            size_t index() const
            {
                return index_;
            }

        private:
            size_t index_;
        };

        class rectlayout
        {
        public:

            rectlayout(unsigned int w, unsigned int h);

            size_t places_count() const;

            size_t placed_rectangles_count() const;

            place get_free_place(size_t n) const;

            bool can_place_rect_at(size_t placeIndex, const indexedrect&) const;

            void place_rect(size_t placeIndex, const indexedrect&);

            vector<placedrect> get_rectangles() const;

            unsigned int width() const
            {
                return width_;
            }

            unsigned int height() const
            {
                return height_;
            }

        private:

            bool can_place_rect(rectangle, const place&) const;

            void remove_free_place(size_t);

            void add_rectangle(const indexedrect&, const place&);

            void add_free_places_created_by(const placedrect&);

            vector<place> get_free_places_created_by(const placedrect& pr) const;

            vector<placedrect_ptr> rectangles_that_span_x(unsigned int) const;

            place get_lowest_place_above(const place&) const;

            bool contains_free_place(const place&) const;

        private:

            vector<place>           free_places_;
            vector<placedrect_ptr>  left_to_right_;
            unsigned int            width_;
            unsigned int            height_;
        };

        inline unsigned int leftmost(const placedrect& r)
        {
            return r.leftmost();
        }

        inline unsigned int rightmost(const placedrect& r)
        {
            return r.rightmost();
        }

        inline unsigned int bottommost(const placedrect& r)
        {
            return r.bottommost();
        }

        inline unsigned int topmost(const placedrect& r)
        {
            return r.topmost();
        }

        inline unsigned int area(unsigned int w, unsigned int h)
        {
            return w * h;
        }

        inline unsigned long area(const rectangle& r)
        {
            return area(r.width(), r.height());
        }

        ////////////////////////////////////////////////////////////////////////////

        rectlayout::rectlayout(unsigned int w, unsigned int h) : width_(w), height_(h)
        {
            free_places_.reserve(256);
            left_to_right_.reserve(256);

            // Initialise the system with the first place to put a rect.
            free_places_.push_back(place(0, 0));
        }

        inline size_t rectlayout::places_count() const
        {
            return free_places_.size();
        }

        vector<placedrect> rectlayout::get_rectangles() const
        {
            vector<placedrect> result;
            result.reserve(placed_rectangles_count());

            for (size_t i = 0; i < left_to_right_.size(); ++i)
            {
                result.push_back(*left_to_right_[i]);
            }

            return result;
        }

        inline size_t rectlayout::placed_rectangles_count() const
        {
            return left_to_right_.size();
        }

        inline place rectlayout::get_free_place(size_t n) const
        {
            return free_places_[n];
        }

        inline void rectlayout::remove_free_place(size_t n)
        {
            free_places_.erase(free_places_.begin() + n);
        }

        //! Overlapping is considered to be when they are not clearly
        //! separated.
        inline bool overlaps(unsigned int A_left,
                             unsigned int A_right,
                             unsigned int B_left,
                             unsigned int B_right)
        {
            if (A_left <= B_left)
            {
                // leftmost
                //
                // A    |----------------|
                // B                  |------|
                //                                 rightmost
                return B_left < A_right;
            }
            else
            {
                // leftmost
                //
                // A                   |------|
                // B     |----------------|
                //                                 rightmost
                return A_left < B_right;
            }
        }

        // Overlapping is considered to be Not-Separate.
        inline bool overlaps_x(const placedrect& A, const placedrect& B)
        {
            return overlaps(leftmost(A), rightmost(A), leftmost(B), rightmost(B));
        }

        inline bool overlaps_y(const placedrect& A, const placedrect& B)
        {
            return overlaps(topmost(A), bottommost(A), topmost(B), bottommost(B));
        }

        bool rectlayout::can_place_rect_at(size_t placeIndex, const indexedrect& r) const
        {
            return can_place_rect(r, get_free_place(placeIndex));
        }

        bool rectlayout::can_place_rect(rectangle r, const place& location) const
        {
            const placedrect pr(0, r, location.x, location.y, false);
            const unsigned int rightx = rightmost(pr);

            if (rightx > width_ || bottommost(pr) > height_)
            {
                return false;
            }

            set<placedrect_ptr> x_overlappers;

            // Go through the left-to-right list up until the first one
            // whose leftmost edge is beyond the tested rectangle's
            // rightmost edge.
            for (auto i = left_to_right_.cbegin();
                 i != left_to_right_.cend() && leftmost(**i) < rightx;
                 ++i)
            {
                if (overlaps_x(pr, **i))
                {
                    x_overlappers.insert(*i);
                }
            }

            // Check if any x over-lappers also overlap in Y...
            for (auto i = x_overlappers.cbegin(); i != x_overlappers.cend(); ++i)
            {
                if (overlaps_y(pr, **i))
                {
                    return false;
                }
            }

            return true;
        }

        bool rectlayout::contains_free_place(const place& p) const
        {
            return find(free_places_.cbegin(), free_places_.cend(), p) != free_places_.cend();
        }

        //! Returns all rectangles where x lies BETWEEN the two corners.
        vector<placedrect_ptr> rectlayout::rectangles_that_span_x(unsigned int x) const
        {
            vector<placedrect_ptr> results;

            auto i = left_to_right_.cbegin();

            while (i != left_to_right_.cend() && (*i)->leftmost() < x)
            {
                if ((*i)->rightmost() > x)
                {
                    // Add this rectangle if the x lies in its span.
                    results.push_back(*i);
                }

                ++i;
            }

            return results;
        }

        //! Returns the lowest place above x,y. This is either the intersection
        //! of the vertical line through x with the top of the bounding
        //! rectangle, or the intersection of the vertical line through x
        //! with the lowest rectangle above and including y.
        place rectlayout::get_lowest_place_above(const place& xy) const
        {
            // Initialise with the intersection of the x and the top of the
            // bounding rect.
            place result(xy.x, 0);

            // Find rects above that span the right corner.
            auto spanningRightSide = rectangles_that_span_x(xy.x);

            if (false == spanningRightSide.empty())
            {
                placedrect_ptr lowestAbove;

                // Find the lowest one that is still above the placed rect.
                for (size_t i = 0; i < spanningRightSide.size(); ++i)
                {
                    if (spanningRightSide[i]->bottommost() <= xy.y)
                    {
                        if ((false == lowestAbove)
                            || (lowestAbove->bottommost() < spanningRightSide[i]->bottommost()))
                        {
                            lowestAbove = spanningRightSide[i];
                        }
                    }
                }

                // We found a lower point than the top of the bounding rect.
                if (lowestAbove)
                {
                    result = place(xy.x, lowestAbove->bottommost());
                }
            }

            return result;
        }

        //! Returns the free places that would be created by this rectangle.
        vector<place> rectlayout::get_free_places_created_by(const placedrect& pr) const
        {
            vector<place> newplaces;
            newplaces.reserve(4);

            // Add top-right and bottom-left corners.
            const place topright = place(pr.rightmost(), pr.topmost());
            const place bottomleft = place(pr.leftmost(), pr.bottommost());

            if (topright.x < width() && topright.y < height())
            {
                newplaces.push_back(topright);
            }

            if (bottomleft.x < width() && bottomleft.y < height())
            {
                newplaces.push_back(bottomleft);
            }

            // Also add the location of the first point above the right corner.
            newplaces.push_back(get_lowest_place_above(topright));

            return newplaces;
        }

        //! Adds the new free places created by the addition of this rectangle.
        void rectlayout::add_free_places_created_by(const placedrect& pr)
        {
            const size_t n = free_places_.size();

            const auto places = get_free_places_created_by(pr);

            // Add only the new places to our list of free places...
            for_each(places.cbegin(),
                     places.cend(),
                     [&](const place& p)
            {
                if (false == contains_free_place(p))
                {
                    free_places_.push_back(p);
                }
            });

            // If we added some, re-sort.
            if (n < free_places_.size())
            {
                auto fn_leftmost_and_highest = [&](const place& lhs, const place& rhs)
                { return lhs.x < rhs.x || ((lhs.x == rhs.x) && lhs.y < rhs.y); };

                sort(free_places_.begin(), free_places_.end(), fn_leftmost_and_highest);
            }
        }

        //! Adds the rectangle at this location and adds the free places
        //! created by its addition.
        void rectlayout::add_rectangle(const indexedrect& r, const place& location)
        {
            placedrect pr(
                r.index(), rectangle(r.width(), r.height()), location.x, location.y, false);

            assert(bottommost(pr) <= height_);
            assert(rightmost(pr) <= width_);

            auto ppr = make_shared<placedrect>(pr);

            add_free_places_created_by(pr);

            // Sort so that the rectangles are listed from by leftmost edge
            // from left to right, and then by length.
            auto sort_by_leftmost = [&](const placedrect_ptr& a, const placedrect_ptr& b)
            {
                return (leftmost(*a) < leftmost(*b))
                       || (leftmost(*a) == leftmost(*b) && a->width() > b->width());
            };

            left_to_right_.insert(
                lower_bound(left_to_right_.begin(), left_to_right_.end(), ppr, sort_by_leftmost),
                ppr);

            assert(false == can_place_rect(r, location));
        }

        //! Requires that the rectangle is known to be placeable at this
        // location.
        void rectlayout::place_rect(size_t placeIndex, const indexedrect& r)
        {
            const place location = get_free_place(placeIndex);

            assert(can_place_rect(r, location));

            // Note that place is removed before adding rect, as this
            // will add new places and invalidate our index.
            remove_free_place(placeIndex);
            add_rectangle(r, location);
        }

        ////////////////////////////////////////////////////////////////////////////

        class solutionset
        {
        public:
            solutionset(const rectlayout& r)
                : rectangles_(r.get_rectangles()), width_(0), height_(0)
            {
                auto bounds = bounding_rectangle(rectangles_);
                width_ = bounds.width();
                height_ = bounds.height();
            }

            const vector<placedrect>& get_rectangles() const
            {
                return rectangles_;
            }

            unsigned int area() const
            {
                return width_ * height_;
            }

            unsigned int width() const
            {
                return width_;
            }

            unsigned int height() const
            {
                return height_;
            }

        private:
            vector<placedrect> rectangles_;
            unsigned int width_;
            unsigned int height_;
        };

        void sort_by_decreasing_heights(vector<indexedrect>& rects)
        {
            sort(rects.begin(),
                 rects.end(),
                 [&](const indexedrect & lhs, const indexedrect & rhs)->bool
            {
                return (lhs.height() > rhs.height())
                       || (lhs.height() == rhs.height() && lhs.width() > rhs.width());
            });
        }

        unsigned int get_total_area(const vector<indexedrect>& rects)
        {
            unsigned int totalArea = 0;

            for_each(rects.cbegin(),
                     rects.cend(),
                     [&](const indexedrect& r)
            { totalArea += area(r); });

            return totalArea;
        }

        unsigned int get_widest_rect(const vector<indexedrect>& rects)
        {
            unsigned int widest = 0;

            for_each(rects.cbegin(),
                     rects.cend(),
                     [&](const indexedrect& r)
            {
                if (widest < r.width())
                {
                    widest = r.width();
                }
            });

            return widest;
        }

        //! A better location is one more leftmost, or equal left and
        //! higher up.
        bool is_better_location(const place& newlocation, const place& oldlocation)
        {
            return (newlocation.x < oldlocation.x)
                   || (newlocation.x == oldlocation.x && newlocation.y < oldlocation.y);
        }

        bool try_place_rect_leftmost(const indexedrect& r, rectlayout& sln)
        {
            bool placed = false;
            size_t placeIndex = 0;
            place location(numeric_limits<unsigned int>::max(),
                           numeric_limits<unsigned int>::max());

            for (size_t i = 0; i < sln.places_count(); ++i)
            {
                if (sln.can_place_rect_at(i, r))
                {
                    auto possibleLocation = sln.get_free_place(i);

                    if (placed)
                    {
                        if (is_better_location(possibleLocation, location))
                        {
                            placeIndex = i;
                            location = possibleLocation;
                        }
                    }
                    else
                    {
                        placed = true;
                        placeIndex = i;
                        location = possibleLocation;
                    }
                }
            }

            if (placed)
            {
                sln.place_rect(placeIndex, r);
            }

            return placed;
        }

        boost::optional<solutionset> try_pack_rects_into_rect(unsigned int w,
                                                              unsigned int h,
                                                              const vector<indexedrect>& rects)
        {
            rectlayout sln(w, h);

            for (size_t i = 0; i < rects.size(); ++i)
            {
                if (false == try_place_rect_leftmost(rects[i], sln))
                {
                    // Rect could not be placed, return empty result.
                    return boost::optional<solutionset>();
                }
            }

            return boost::optional<solutionset>(solutionset(sln));
        }

        solutionset find_initial_solution(const vector<indexedrect>& rects)
        {
            auto sln = try_pack_rects_into_rect(
                numeric_limits<unsigned int>::max(), rects[0].height(), rects);

            // It should be impossible to NOT fit the rectangles in.
            assert(sln);

            return *sln;
        }

        //! Reduced the w and increases the height until an area is found
        //! that is at least as big as the given minimum one.
        void make_longer_and_narrower(unsigned int& w,
                                      unsigned int& h,
                                      const unsigned int minimumArea)
        {
            assert(w > 1);

            --w;

            while (area(w, h) < minimumArea)
            {
                ++h;
            }
        }

        solutionset find_best_fit(vector<indexedrect> rects)
        {
            sort_by_decreasing_heights(rects);

            const unsigned int minimumArea = get_total_area(rects);

            const unsigned int widestRect = get_widest_rect(rects);

            solutionset bestsofar = find_initial_solution(rects);

            unsigned int currentwidth = bestsofar.width();
            unsigned int currentheight = bestsofar.height();

            make_longer_and_narrower(currentwidth, currentheight, minimumArea);

            while (currentwidth > widestRect)
            {
                if (area(currentwidth, currentheight) < bestsofar.area())
                {
                    // clog << "Testing " << currentwidth << " x " <<
                    // currentheight << "\n";

                    // See if we can fit rectangles into the new smaller
                    // area....
                    boost::optional<solutionset> fittedrects
                        = try_pack_rects_into_rect(currentwidth, currentheight, rects);

                    if (fittedrects)
                    {
                        // They fitted.
                        if (fittedrects->area() < bestsofar.area())
                        {
                            bestsofar = *fittedrects;
                        }

                        make_longer_and_narrower(currentwidth, currentheight, minimumArea);
                    }
                    else
                    {
                        // We could not fit them into current rect...make it
                        // taller.
                        ++currentheight;
                    }
                }
                else
                {
                    make_longer_and_narrower(currentwidth, currentheight, minimumArea);
                }
            }

            return bestsofar;
        }
    }

    //////////////////////////////////////////////////////////////////////////

    //! Public ctor.
    rectangle::rectangle(unsigned int w, unsigned int h) : width_(w), height_(h)
    {
        if (w == 0 || h == 0)
        {
            ss_throw("Zero area bestfit::rectangle.");
        }
    }

    //! Returns a rotated version of the rectangle.
    rectangle rectangle::rotated() const
    {
        return rectangle(height_, width_);
    }

    //////////////////////////////////////////////////////////////////////////

    //! Public ctor.
    placedrect::placedrect(
        size_t srcIndex, rectangle r, unsigned int cornerX, unsigned int cornerY, bool rotate)
        : index_(srcIndex)
        , rect_(rotate ? r.rotated() : r)
        , xy_(cornerX, cornerY)
        , wasRotated_(rotate)
    {
    }

    position2d<unsigned int> placedrect::position() const
    {
        return xy_;
    }

    bool placedrect::is_rotated() const
    {
        return wasRotated_;
    }

    unsigned int placedrect::leftmost() const
    {
        return xy_.x();
    }

    unsigned int placedrect::topmost() const
    {
        return xy_.y();
    }
    unsigned int placedrect::rightmost() const
    {
        return leftmost() + rect_.width();
    }

    unsigned int placedrect::bottommost() const
    {
        return topmost() + rect_.height();
    }
    unsigned int placedrect::width() const
    {
        return rect_.width();
    }
    unsigned int placedrect::height() const
    {
        return rect_.height();
    }

    size_t placedrect::index() const
    {
        return index_;
    }

    //---------------------------------------------------------------------

    vector<indexedrect> copy_src_rects(const vector<rectangle>& rects);
    vector<placedrect> best_fit_rectangle_containing(const vector<rectangle>& srcrects);
    rectangle bounding_rectangle(const std::vector<placedrect>& rects);
    void save_bestfit_as_bitmap(const char* filename, const std::vector<bestfit::placedrect>& fit);

    vector<indexedrect> copy_src_rects(const vector<rectangle>& rects)
    {
        vector<indexedrect> result;
        result.reserve(rects.size());

        size_t n = 0;
        transform(rects.begin(),
                  rects.end(),
                  back_inserter(result),
                  [&](const rectangle& r)
        { return indexedrect(r, n++); });

        return result;
    }

    //! Searches for good smallest bounding rectangle which can enclose the
    //! given rectangles.
    vector<placedrect> best_fit_rectangle_containing(const vector<rectangle>& srcrects)
    {
        if (srcrects.empty())
        {
            return vector<placedrect>();
        }

        return find_best_fit(copy_src_rects(srcrects)).get_rectangles();
    }

    //! Assuming that the rectangles begin with one at 0,0, finds the
    //! rectangle that encloses them.
    rectangle bounding_rectangle(const std::vector<placedrect>& rects)
    {
        unsigned int x = 0;
        unsigned int y = 0;

        for_each(rects.cbegin(),
                 rects.cend(),
                 [&](const placedrect& r)
        {
            assert(leftmost(r) <= rightmost(r));
            assert(topmost(r) <= bottommost(r));

            if (x < rightmost(r))
            {
                x = rightmost(r);
            }

            if (y < bottommost(r))
            {
                y = bottommost(r);
            }
        });

        return rectangle(x, y);
    }

    void save_bestfit_as_bitmap(const char* filename, const std::vector<bestfit::placedrect>& fit)
    {
        if (filename && false == fit.empty())
        {
            auto bestrect = bounding_rectangle(fit);

            const float widthf = static_cast<float>(bestrect.width());
            const float heightf = static_cast<float>(bestrect.height());
            const float nf = static_cast<float>(fit.size());

            const colour clrA(0.95f, 1.0f, 0.9f);
            const colour clrB(0.15f, 0.25f, 0.1f);

            image bmp(bestrect.width(), bestrect.height(), image::format_bgr);

            bmp.clear_to_bgra(0, 0, 255, 0);

            vector<colour> colours;
            colours.reserve(fit.size());

            for (size_t i = 0; i < fit.size(); ++i)
            {
                const float vary = (nf > 1.0f) ? (i / (nf - 1.0f)) : 0.0f;
                const auto clr = clrA.interpolate(vary, clrB);
                colours.push_back(clr);
            }

            random_shuffle(colours.begin(), colours.end());

            for (size_t i = 0; i < fit.size(); ++i)
            {
                const auto clr = colours[i];

                const auto& r = fit[i];

                const float x1 = r.leftmost() / widthf;
                const float x2 = r.rightmost() / widthf;
                const float y1 = r.topmost() / heightf;
                const float y2 = r.bottommost() / heightf;

                bmp.draw_triangle(x1, y1, clr, x2, y1, clr, x2, y2, clr);

                bmp.draw_triangle(x1, y1, clr, x1, y2, clr, x2, y2, clr);
            }

            bmp.save(filename);
        }
    }
}
}
