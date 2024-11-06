#ifndef solosnake_frameduration_hpp
#define solosnake_frameduration_hpp
#include "solosnake/throw.hpp"

namespace solosnake
{
    class frameduration
    {
    public:
        explicit frameduration( unsigned int ms ) : frame_duration_ms_( ms )
        {
            if( ms == 0 )
            {
                ss_throw( "frame duraction cannot be zero ms." );
            }
        }

        unsigned int in_milliseconds() const
        {
            return frame_duration_ms_;
        }

        float in_seconds() const
        {
            return ( in_milliseconds() / 1000.0f );
        }

        float frames_per_second() const
        {
            return ( 1000.0f / in_milliseconds() );
        }

    private:
        unsigned int frame_duration_ms_;
    };
}

#endif
