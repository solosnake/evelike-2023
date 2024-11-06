#ifndef blue_fxsense_hpp
#define blue_fxsense_hpp

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
    struct Sensed;
    class fxsound;
    class ifxrenderer;

    //! A sfx for when a Machine senses: a ripple which spreads out and fades.
    class fxsense : public ifx
    {
    public:

        fxsense(
            float sense_board_height,
            ifxrenderer& );

        virtual ~fxsense();

        void restart( Sensed s );

        static std::unique_ptr<ifxrenderer> make_fxrenderer(
            const std::shared_ptr<solosnake::rendering_system>&,
            const std::shared_ptr<solosnake::sound_renderer>&,
            const solosnake::persistance& );

    private:

        class fxsenserenderer;
        class fxsensedata;

        const void* event_data() const override
        {
            return data_.get();
        }

        std::unique_ptr<fxsensedata> data_;
    };
}

#endif
