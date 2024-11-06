#ifndef solosnake_fontfamily_hpp
#define solosnake_fontfamily_hpp

#include <memory>
#include <vector>

namespace solosnake
{
    class font;

    //! Manages a set of fonts which represent the same font but have different
    //! sizes. Call this with a size and it will return the best font to use
    //! with a font printer to draw text with.
    class fontfamily
    {
    public:

        typedef std::pair<std::shared_ptr<font>, float> scaled_font_t;
        typedef std::pair<std::shared_ptr<font>, int>   sized_font_t;

        void add_sized_font( const sized_font_t& );

        scaled_font_t get_scaled_font( int size ) const;

        std::shared_ptr<font> get_font_nearest_size( int size ) const;

        bool empty() const;

    private:
        std::vector<sized_font_t> fontsizes_;
    };

    //-------------------------------------------------------------------------

    inline bool fontfamily::empty() const
    {
        return fontsizes_.empty();
    }
}

#endif
