#ifndef solosnake_nextscreen_hpp
#define solosnake_nextscreen_hpp

#include <algorithm>
#include <string>
#include "solosnake/filepath.hpp"
#include "solosnake/nothrow.hpp"

namespace solosnake
{
    //! Returned from a screen after its display has ended, decides what
    //! the next screen will be.
    class nextscreen
    {
    public:

        nextscreen();

        nextscreen( const nextscreen& );

        nextscreen( nextscreen&& ) SS_NOEXCEPT;

        nextscreen( const std::string& name );

        const std::string& screen_name() const;

        operator bool() const;

        nextscreen& operator=( const nextscreen& );

        nextscreen& operator=( nextscreen && ) SS_NOEXCEPT;

    private:

        std::string screen_name_;
    };

    //-------------------------------------------------------------------------

    inline nextscreen::nextscreen( const nextscreen& other ) : screen_name_( other.screen_name_ )
    {
    }

    inline nextscreen::nextscreen( nextscreen&& other ) SS_NOEXCEPT
        : screen_name_( std::move( other.screen_name_ ) )
    {
    }

    inline nextscreen& nextscreen::operator=( const nextscreen& other )
    {
        if( this != &other )
        {
            screen_name_ = other.screen_name_;
        }

        return *this;
    }

    inline nextscreen& nextscreen::operator=( nextscreen && other ) SS_NOEXCEPT
    {
        if( this != &other )
        {
            screen_name_ = std::move( other.screen_name_ );
        }

        return *this;
    }
}

#endif
