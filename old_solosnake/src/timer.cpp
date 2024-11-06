#include <cassert>
#include <ctime>
#include <chrono>
#include "solosnake/timer.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"

namespace
{
    // Can be any clock type. (steady_clock or high_resolution_clock).
    template<typename T> 
    class chronotimer : public solosnake::timer
    {
    public:

        typedef T clock_t;
        typedef typename clock_t::time_point time_point_t;

        chronotimer() : start_( clock_t::now() )
        {
        }

        virtual ~chronotimer()
        {
        }

        virtual float elapsed() const
        {
            return std::chrono::duration_cast<std::chrono::duration<float>>( clock_t::now() - start_ ).count();
        }

        virtual float restart()
        {
            const float dt = this->elapsed();
            start_ = clock_t::now();
            return dt;
        }

    private:
        time_point_t start_;
    };

}

namespace solosnake
{
    // Virtual dtor implementation.
    timer::~timer()
    {
    }

    std::unique_ptr<timer> make_timer()
    {
        std::unique_ptr<timer> t;

        if( std::chrono::high_resolution_clock::is_steady )
        {
            ss_log( "Using high_resolution_clock. Precision is ",
                    std::chrono::high_resolution_clock::period::num,
                    "/",
                    std::chrono::high_resolution_clock::period::den,
                    " seconds" );

            t.reset( new chronotimer<std::chrono::high_resolution_clock>() );
        }
        else
        {
            ss_log( "Using high_resolution_clock. Precision is ",
                    std::chrono::steady_clock::period::num,
                    "/",
                    std::chrono::steady_clock::period::den,
                    " seconds" );

            t.reset( new chronotimer<std::chrono::steady_clock>() );
        }

        return t;
    }
}
