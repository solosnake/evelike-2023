#ifndef solosnake_textureanimationtimeline_hpp
#define solosnake_textureanimationtimeline_hpp

#include <algorithm>
#include <cstdint>
#include <cassert>
#include <vector>
#include <utility>

namespace solosnake
{
    //! Stateless const timeline, just a series of numbers. Supports blending
    //! between them.
    class textureanimationtimeline
    {
    public:
        textureanimationtimeline();

        explicit textureanimationtimeline( const std::vector<std::uint8_t>& );

        bool has_same_values( const std::vector<std::uint8_t>& values ) const;

        bool operator<( const textureanimationtimeline& rhs ) const;

        bool operator<( const std::vector<std::uint8_t>& rhs ) const;

        unsigned int animation_duration_ms() const;

        unsigned int next_time( const unsigned int nowMs, const unsigned int dtMs ) const;

        std::uint8_t value_at_time( const unsigned int ms ) const;

    private:
        std::vector<std::uint8_t> timeline_values_;
    };

    //-------------------------------------------------------------------------

    inline bool textureanimationtimeline::has_same_values( const std::vector<std::uint8_t>& values ) const
    {
        return timeline_values_ == values;
    }

    inline bool textureanimationtimeline::operator<( const textureanimationtimeline& rhs ) const
    {
        return timeline_values_ < rhs.timeline_values_;
    }

    inline bool textureanimationtimeline::operator<( const std::vector<std::uint8_t>& rhs ) const
    {
        return timeline_values_ < rhs;
    }

    inline unsigned int textureanimationtimeline::animation_duration_ms() const
    {
        return std::max<unsigned int>( 250u, static_cast<unsigned int>( 250u * timeline_values_.size() ) );
    }

    inline unsigned int textureanimationtimeline::next_time( const unsigned int nowMs,
            const unsigned int dtMs ) const
    {
        return ( nowMs + dtMs ) % animation_duration_ms();
    }

    inline std::uint8_t textureanimationtimeline::value_at_time( const unsigned int ms ) const
    {
        if( timeline_values_.empty() )
        {
            // Return the "WHITE" signal when there are no values. This is
            // intended to drive the animation to be always on.
            return 0xFF;
        }
        else
        {
            const unsigned int values_count = static_cast<unsigned int>( timeline_values_.size() );
            const unsigned int duration = 250u * values_count;
            const unsigned int clampToMax = std::min( ms, duration - 1 );
            const unsigned int index = ( clampToMax * values_count ) / duration;
            assert( index < timeline_values_.size() );
            return timeline_values_[index];
        }
    }
}

#endif
