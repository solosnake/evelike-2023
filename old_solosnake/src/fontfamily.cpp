#include <cassert>
#include <limits>
#include "solosnake/fontfamily.hpp"

namespace solosnake
{
    namespace
    {
        static inline long abs_diff( int x, int y )
        {
            return x > y ? ( x - y ) : ( y - x );
        }

        //! Returns a factor which is equivalent to a font being scaled up n times.
        //! n must be greater than zero.
        inline float scale_up( int i )
        {
            assert( i > 0 );

            float n = 1.0f;
            float d = 1.0f;

            do
            {
                n *= 3.0f;
                d *= 2.0f;
            }
            while( --i );

            return n / d;
        }

        //! Returns a factor which is equivalent to a font being scaled down i times.
        //! i must be greater than zero.
        inline float scale_down( int i )
        {
            assert( i > 0 );

            float n = 1.0f;
            float d = 1.0f;

            do
            {
                n *= 2.0f;
                d *= 3.0f;
            }
            while( --i );

            return n / d;
        }

        fontfamily::scaled_font_t scale_font( const fontfamily::sized_font_t& f, int size )
        {
            fontfamily::scaled_font_t sf;

            sf.first = f.first;
            sf.second = 1.0f;

            if( size != f.second )
            {
                sf.second = ( size > f.second ) ? scale_up( size - f.second ) : scale_down( f.second - size );
            }

            return sf;
        }
    }

    //-------------------------------------------------------------------------

    void fontfamily::add_sized_font( const sized_font_t& f )
    {
        fontsizes_.push_back( f );
    }

    fontfamily::scaled_font_t fontfamily::get_scaled_font( int size ) const
    {
        assert( false == fontsizes_.empty() );

        if( fontsizes_.size() == 1 || size < fontsizes_.front().second )
        {
            // There is only one font, or the requested size is smaller than
            // the first font.
            return scale_font( fontsizes_.front(), size );
        }
        else if( size > fontsizes_.back().second )
        {
            return scale_font( fontsizes_.back(), size );
        }

        // Find nearest. We know now the size is in the range, and we know
        // there is more than one. Look at pairs for a range that contains
        // the size.
        size_t ibest = std::numeric_limits<size_t>::max();
        for( size_t i = 1; i < fontsizes_.size(); ++i )
        {
            if( fontsizes_[i - 1].second == size )
            {
                ibest = i = 1;
                break;
            }
            else if( fontsizes_[i - 1].second < size && fontsizes_[i].second >= size )
            {
                // Find the size it is closest to.
                ibest = ( size - fontsizes_[i - 1].second ) < ( fontsizes_[i - 1].second - size ) ? i - 1
                        : i;
                break;
            }
        }

        assert( ibest < fontsizes_.size() );

        return scale_font( fontsizes_[ibest], size );
    }

    //! Returns the index of the font whose size is nearest the given size.
    std::shared_ptr<font> fontfamily::get_font_nearest_size( int size ) const
    {
        assert( false == empty() );

        size_t result = 0;

        long smallestDiff = abs_diff( size, fontsizes_.at( 0 ).second );

        for( size_t i = 1; i < fontsizes_.size(); ++i )
        {
            auto d = abs_diff( size, fontsizes_[i].second );
            if( d < smallestDiff )
            {
                smallestDiff = d;
                result = i;
            }
        }

        return fontsizes_.at( result ).first;
    }
}
