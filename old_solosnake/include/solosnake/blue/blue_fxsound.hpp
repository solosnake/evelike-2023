#ifndef blue_fxsound_hpp
#define blue_fxsound_hpp

#include <memory>
#include "solosnake/iworldobject.hpp"
#include "solosnake/sound_renderer.hpp"

namespace blue
{
    //! Class which wraps the requirements of moving sound associated with
    //! an fx. This object must only be created on the heap.
    class fxsound :
        public solosnake::iworldobject,
        public std::enable_shared_from_this<fxsound>
    {
        struct HeapOnly
        {
        };

    public:


        fxsound( const float y, const HeapOnly );

        virtual ~fxsound();

        void reset( const float x, const float z );

        void play_sound( solosnake::sound_renderer* const, const solosnake::sound_renderer::BufferHandle );

        static std::shared_ptr<fxsound> make_fxsound( const float y );

    private:

        float sound_volume_;
        float sound_radius_;
    };
}

#endif
