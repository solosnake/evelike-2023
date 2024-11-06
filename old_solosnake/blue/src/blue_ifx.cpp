#include "solosnake/blue/blue_ifx.hpp"
#include "solosnake/blue/blue_ifxrenderer.hpp"

namespace blue
{
    ifx::ifx( GameEffectType t, ifxrenderer& r )
        : renderer_( &r )
        , effect_type_( t )
        , duration_ms_( 0u )
        , current_ms_( 0u )
        , started_( false )
    {
        worldXZ_[0] = worldXZ_[1] = 0.0f;
    }

    ifx::~ifx()
    {
    }

    void ifx::restart( const float xz[2], unsigned int duration )
    {
        duration_ms_ = duration;
        current_ms_ = 0u;
        worldXZ_[0] = xz[0];
        worldXZ_[1] = xz[1];
        started_ = false;
    }

    SfxState ifx::update( bool advanceAnimations, const unsigned int dtms )
    {
        if( advanceAnimations && current_ms_ < duration_ms_ )
        {
            current_ms_ += dtms;
        }

        return current_ms_ < duration_ms_ ? EffectIsActive : EffectIsFinished;
    }

    void ifx::draw_sfx()
    {
        float ms = static_cast<float>( current_ms_ );
        float timeline = ms / static_cast<float>( duration_ms_ );

        bool justStarted = !started_;

        if( justStarted )
        {
            started_ = true;
        }

        renderer_->draw_fx( timeline, ms, worldXZ_, justStarted, *this );
    }

    const void* ifx::event_data() const
    {
        return nullptr;
    }
}
