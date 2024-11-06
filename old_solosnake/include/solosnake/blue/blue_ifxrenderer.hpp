#ifndef blue_isfxrenderer_hpp
#define blue_isfxrenderer_hpp

namespace blue
{
    class ifx;

    //! This is the type of effect to which rendering is deferred. This class
    //! is an fx 'renderer' in that it is responsible for drawing art and
    //! sounds etc associated with an fx.
    class ifxrenderer
    {
        friend class ifx;

        virtual void draw_fx(
            const float timeline,
            const float ms,
            const float worldXZ[2],
            const bool justStarted,
            const ifx& ) = 0;

    public:

        virtual ~ifxrenderer();
    };
}

#endif
