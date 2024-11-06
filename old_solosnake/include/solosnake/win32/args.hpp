#ifndef solosnake_args_hpp
#define solosnake_args_hpp

#ifdef WIN32

#include <string>
#include <vector>

namespace solosnake
{
    // WIN32 ONLY.
    class args
    {
    public:
        args();

        args( int argc, const char* argv[] );

        int argc() const;

        const std::string& argv( size_t n ) const;

        const char* const* argvs() const;

    private:
        std::vector<std::string> args_;
        std::vector<const char*> argvs_;
    };
}

#endif
#endif