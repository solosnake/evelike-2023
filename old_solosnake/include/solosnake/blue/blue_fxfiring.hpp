#ifndef blue_fxfiring_hpp
#define blue_fxfiring_hpp

#include <memory>
#include "solosnake/blue/blue_ifx.hpp"

namespace solosnake
{
    class persistance;
    class sound_renderer;
    class rendering_system;
}

namespace blue
{
    struct Shot_fired;
    class fxsound;
    class ifxrenderer;

    //! A SFX for when a Machine fires its Hardpoint weapon (HardpointType enum is WeaponHardpoint) at something.
    class fxweaponfiring : public ifx
    {
    public:

        fxweaponfiring( float y, ifxrenderer& );

        virtual ~fxweaponfiring();

        void restart( const Shot_fired& );

        static std::unique_ptr<ifxrenderer> make_fxrenderer(
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

    private:

        class fxfiringrenderer;
        class fxfiringdata;

        const void* event_data() const override
        {
            return data_.get();
        }

        std::unique_ptr<fxfiringdata>    data_;
    };
}

#endif
