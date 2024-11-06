#ifndef solosnake_timer_hpp
#define solosnake_timer_hpp

#include <memory>

namespace solosnake
{
    // A high precision steady seconds-resolution timer interface.
    class timer
    {
    public:

        virtual ~timer();

        //! Smallest elapsed value is 1 ms. Zero is never returned.
        inline unsigned long elapsed_milliseconds() const
        {
            const unsigned long dt = static_cast<unsigned long>( 1000.0f * this->elapsed() );
            return dt > 0L ? dt : 1L;
        }

        //! Returns the delta in seconds since the last call to delta or to restart.
        //! This call restarts the timer each time.
        inline float delta()
        {
            float d = elapsed();
            restart();
            return d;
        }

        // Returns elapsed time in seconds and sets elapsed to 0.
        virtual float restart() = 0;

        // Returns elapsed time in seconds.
        virtual float elapsed() const = 0;
    };

    // Returns a steady high precision seconds timer on this system.
    std::unique_ptr<timer> make_timer();
}

#endif
