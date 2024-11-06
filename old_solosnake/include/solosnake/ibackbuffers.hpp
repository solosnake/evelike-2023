#ifndef solosnake_irenderer_hpp
#define solosnake_irenderer_hpp
#include <memory>

namespace solosnake
{
    //! Base class for renderer implementations.
    class ibackbuffers : public std::enable_shared_from_this<ibackbuffers>
    {
    public:

        virtual ~ibackbuffers();

        virtual void swap_buffers() = 0;
    };
}

#endif
