#ifndef solosnake_iscreenstate_hpp
#define solosnake_iscreenstate_hpp

#include <memory>
#include "solosnake/nextscreen.hpp"

namespace solosnake
{
    //! Interface that should return when the user has requested to
    //! end / leave the current screen, and which screen the
    //! to move to.
    class iscreenstate : public std::enable_shared_from_this<iscreenstate>
    {
    public:

        virtual bool is_screen_ended() const = 0;

        virtual nextscreen get_next_screen() const = 0;

        virtual ~iscreenstate();
    };
}

#endif
