#ifndef blue_fxmachineexplosion_hpp
#define blue_fxmachineexplosion_hpp

#include <memory>
#include "solosnake/blue/blue_ifx.hpp"

namespace solosnake
{
    class persistance;
    class rendering_system;
    class sound_renderer;
}

namespace blue
{
    struct Machine_exploded;
    class ifxrenderer;

    //! A sfx for when a Machine explodes.
    class fxmachineexplosion : public ifx
    {
    public:

        fxmachineexplosion( const float y, ifxrenderer& );

        ~fxmachineexplosion();

        void restart( Machine_exploded );

        static std::unique_ptr<ifxrenderer> make_fxrenderer(
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance&);

    private:

        class fxmachineexplosionrenderer;
        class fxmachineexplosiondata;

        const void* event_data() const override
        {
            return data_.get();
        }

        std::unique_ptr<fxmachineexplosiondata> data_;
    };
}

#endif
