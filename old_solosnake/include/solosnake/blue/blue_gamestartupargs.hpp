#ifndef blue_gamestartupargs_hpp
#define blue_gamestartupargs_hpp

#include <memory>
#include <string>

namespace blue
{
    //! This is the info sent across the network which is used to initialise the
    //! game on all clients.
    class gamestartupargs
    {
    public:

        gamestartupargs(
            unsigned int seed,
            const std::string& boardname );

        explicit gamestartupargs( const char* );

        std::string to_str() const;

        bool operator==( const gamestartupargs& ) const;

        bool operator!=( const gamestartupargs& ) const;

        unsigned int random_seed() const
        {
            return rand_seed_;
        }

    private:

        unsigned int    rand_seed_;
        std::string     boardname_;
    };
}

#endif
