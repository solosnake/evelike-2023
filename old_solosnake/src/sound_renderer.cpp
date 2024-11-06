#include "solosnake/sound_renderer.hpp"
#include "solosnake/iworldobject.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/unreachable.hpp"
#include "solosnake/external/openal.hpp"
#include <cassert>
#include <algorithm>
#include <cmath>

#define SS_STRINGIFY(x) #x
#define SS_TOSTRING(x) SS_STRINGIFY(x)
#define SS_ERR_LOCATION __FILE__ ":" SS_TOSTRING(__LINE__)

#define SOLOSNAKE_SPEED_OF_SOUND_IN_AIR 343.3f

//! A sound has a bubble, and when the sound is closer than the bubble radius
//! times this factor, it is no longer attenuated but is at full volume. */
#define SOLOSNAKE_SOUND_RADIUS_REF_FACTOR 0.05f

//! To stop clicking when we cull sounds we slightly expand their bubble radius,
//! giving us a pessimistic volume. */
#define SOLOSNAKE_SOUND_RADIUS_EXPANSION 1.05f

#ifndef NDEBUG
#define SOLOSNAKE_ASSERT_OPENAL_OK check_openal(SS_ERR_LOCATION);
#else
#define SOLOSNAKE_ASSERT_OPENAL_OK
#endif

namespace solosnake
{
    namespace
    {

#ifndef NDEBUG // We get a warning from gcc because these functions are unused in release builds.
        
        void check_openal( const char* line )
        {
            auto e = alGetError();
            if( e != AL_NO_ERROR )
            {
                ss_err( "OpenAL error: ", line, alGetString( e ) );
                // assert( ! "OpenAL error encountered." );
            }
        }

        bool is_playing( ALuint src )
        {
            ALint status;
            alGetSourcei( src, AL_SOURCE_STATE, &status );
            return status == AL_PLAYING;
        }

#endif // NDEBUG

        void free_buffers( const std::vector<ALuint>& buffers )
        {
            for( size_t i = 0; i < buffers.size(); ++i )
            {
                alDeleteBuffers( 1, &buffers[i] );
            }
        }

        // Use status == AL_STOPPED to check for stopped.
        // Use status == AL_INITIAL to check for initial (?)

        void update_iworldobject_sound( ALuint src, const iworldobject& w )
        {
            SOLOSNAKE_ASSERT_OPENAL_OK;

            if( w.is_valid() )
            {
                alSourcefv( src, AL_POSITION, w.location().xyz );
                alSourcefv( src, AL_VELOCITY, w.velocity().xyz );
            }
            else
            {
                alSourceStop( src );
            }

            SOLOSNAKE_ASSERT_OPENAL_OK;
        }
    }

    sound_renderer::Sound::Sound() : bh( 0 ), vol( 0.0f ), radius( 0.0f ), loop( false )
    {
    }

    sound_renderer::SoundSrc::SoundSrc() : attached(), src( 0 ), details()
    {
    }

    sound_renderer::Sleeping::Sleeping() : attached(), snd()
    {
    }

    //! Constructs a sound renderer (using OpenAL).
    //! By default the renderer created has a listener at 0,0,0 and facing the
    //! same direction as the forward vector in the identity matrix.
    //! @param worldScale How many times bigger or smaller one distance unit is
    //! to a metre. If you are using metres as your coordinates, then this should
    //! be 1.0f. If you are using kilometres as your unit, this should be 1000.0f.
    //! @param maxSounds The maximum number of sounds that can be played at once.
    sound_renderer::sound_renderer( std::shared_ptr<ifilefinder> ff,
                                    sound_renderer::FalloffModel falloff,
                                    float worldScale,
                                    size_t maxSounds )
        : files_( ff )
        , buffers_()
        , sources_()
        , sleeping_()
        , nplaying_( 0 )
        , soundVolLowerLimit_( 0.0f )
        , fvolumecalc_( nullptr )
    {
        assert( ff );

        if( maxSounds == 0 )
        {
            ss_throw( "Max sounds must be greater than zero." );
        }

        if( worldScale < 0.0f )
        {
            ss_throw( "Invalid world scale. World scale must be > 0." );
        }

        solosnake::alut_init();
        SOLOSNAKE_ASSERT_OPENAL_OK;

        ALenum e = alGetError();
        assert( e == AL_NO_ERROR );

        switch( falloff )
        {
            case sound_renderer::LinearFalloff:
                alDistanceModel( AL_LINEAR_DISTANCE_CLAMPED );
                fvolumecalc_ = &sound_renderer::volume_in_linear;
                break;

            case sound_renderer::InverseFalloff:
                alDistanceModel( AL_INVERSE_DISTANCE_CLAMPED );
                fvolumecalc_ = &sound_renderer::volume_in_inverse;
                break;

            case sound_renderer::ExponentialFalloff:
                alDistanceModel( AL_EXPONENT_DISTANCE_CLAMPED );
                fvolumecalc_ = &sound_renderer::volume_in_exponential;
                break;

            default:
                ss_unreachable;
        }

        // Setup defaults:
        matrix4x4_t m4x4;
        load_identity_4x4( m4x4 );

        // Facing down z axis.
        set_listener_orientation( location_4x4( m4x4 ), up_4x4( m4x4 ), forward_4x4( m4x4 ) );

        alListener3f( AL_VELOCITY, 0.0f, 0.0f, 0.0f );
        alListenerf( AL_GAIN, 1.0f );
        alDopplerFactor( 1.0f );
        alSpeedOfSound( SOLOSNAKE_SPEED_OF_SOUND_IN_AIR / worldScale );

        // Reserve/allocate vectors.
        SoundSrc dummy;
        sources_.resize( maxSounds, dummy );
        buffers_.reserve( std::max<size_t>( 2 * maxSounds, 64 ) );
        sleeping_.reserve( std::max<size_t>( 4 * maxSounds, 64 ) );

        // Generate sources:
        for( size_t i = 0; i < maxSounds; ++i )
        {
            alGenSources( static_cast<ALsizei>( 1 ), &sources_[i].src );
            e = alGetError();
            if( e != AL_NO_ERROR )
            {
                if( e == AL_INVALID_VALUE )
                {
                    // We have exhausted the OpenAL resources. Stop
                    // here and resize the buffers.
                    sources_.resize( i );
                    break;
                }
                else
                {
                    ss_throw( "An error occured during creation of sound renderer." );
                }
            }
        }

        e = alGetError();
        if( e )
        {
            ss_err( SS_ERR_LOCATION "OpenAL error : ", alGetString( e ) );
            ss_throw( "An error occured during creation of sound renderer." );
        }

        SOLOSNAKE_ASSERT_OPENAL_OK;
    }

    sound_renderer::~sound_renderer()
    {
        SOLOSNAKE_ASSERT_OPENAL_OK;
        nplaying_ = 0;

        sleeping_.clear();

        for( size_t i = 0; i < sources_.size(); ++i )
        {
            alDeleteSources( 1, &sources_[i].src );
        }
        sources_.clear();

        SOLOSNAKE_ASSERT_OPENAL_OK;

        free_buffers( buffers_ );
        buffers_.clear();

        SOLOSNAKE_ASSERT_OPENAL_OK;

        solosnake::alut_release();
    }

    //! Updates the playing sounds. This should be called inside the game loop.
    void sound_renderer::update()
    {
        SOLOSNAKE_ASSERT_OPENAL_OK;

        if( nplaying_ > 0 )
        {
            ALint status;

            size_t i = 0u;
            size_t f = nplaying_ - 1u; // Index of 1st free src.
            size_t n = nplaying_;
            while( n-- )
            {
                SoundSrc& sound = sources_[i];

                alGetSourcei( sound.src, AL_SOURCE_STATE, &status );

                if( status == AL_STOPPED )
                {
                    --nplaying_;

                    // Detach from buffer and remove attachment (if any).
                    alSourcei( sound.src, AL_BUFFER, 0 );
                    sound.attached.reset();

                    if( i != f )
                    {
                        auto dead = sources_[i];
                        sources_[i] = sources_[f];
                        sources_[f] = dead;
                        --f;
                    }
                }
                else
                {
                    if( sound.attached )
                    {
                        if( should_be_sleeping( sound ) )
                        {
                            sleep_sound( sound );
                        }
                        else
                        {
                            // Dynamically positioned sound.
                            // Update its coordinates, or even kill the sound if
                            // the object is dead. If killed, it will be
                            // automatically
                            // removed next update!
                            update_iworldobject_sound( sound.src, *sound.attached );
                        }
                    }

                    // Move to next sound in array:
                    ++i;
                }
            }
        }

        // Check to see if any sounds should wake up:
        if( nplaying_ < sources_.size() && !sleeping_.empty() )
        {
            size_t n = sleeping_.size();
            size_t i = 0u;
            while( n-- )
            {
                Sleeping& sleeper = sleeping_[i];

                if( !sleeper.attached->is_valid() )
                {
                    // World object has become invalid. Remove sleeper.
                    sleeping_[i] = sleeping_[n];
                    sleeping_.resize( n );
                }
                else if( should_be_awake( sleeper ) )
                {
                    play_buffer( sleeper.snd.bh,
                                 sleeper.attached,
                                 sleeper.snd.vol,
                                 sleeper.snd.radius,
                                 sleeper.snd.loop );

                    // Swap out removed sleeper.
                    sleeping_[i] = sleeping_[n];
                    sleeping_.resize( n );
                }
                else
                {
                    ++i;
                }
            }
        }

        SOLOSNAKE_ASSERT_OPENAL_OK;
    }

    float sound_renderer::volume_in_inverse( float d, float ref, float mx, float roll )
    {
        d = std::max<float>( ref, d );
        d = std::min<float>( mx, d );
        return ref / ( ref + roll * ( d - ref ) );
    }

    float sound_renderer::volume_in_linear( float d, float ref, float mx, float roll )
    {
        d = std::max<float>( ref, d );
        d = std::min<float>( mx, d );
        return ( 1.0f - roll * ( d - ref ) / ( mx - ref ) );
    }

    float sound_renderer::volume_in_exponential( float d, float ref, float mx, float roll )
    {
        d = std::max<float>( ref, d );
        d = std::min<float>( mx, d );
        return std::pow( ( d / ref ), ( -roll ) );
    }

    bool sound_renderer::should_be_awake( const sound_renderer::Sleeping& s ) const
    {
        assert( s.snd.loop && s.attached );

        return ( ( *fvolumecalc_ )( distance3( s.attached->location().xyz, listenerxyz_ ),
                                    s.snd.radius * SOLOSNAKE_SOUND_RADIUS_REF_FACTOR
                                    * SOLOSNAKE_SOUND_RADIUS_EXPANSION,
                                    s.snd.radius * SOLOSNAKE_SOUND_RADIUS_EXPANSION,
                                    1.0f ) * s.snd.vol ) > soundVolLowerLimit_;
    }

    void sound_renderer::sleep_sound( const SoundSrc& s )
    {
        assert( s.details.loop );
        assert( s.attached );
        alSourceStop( s.src );
        Sleeping sleeper( s.details, s.attached );
        sleeping_.push_back( sleeper );
    }

    //! Checks to see how far the sound is from the listener, taking into account
    //! its volume, and returns true if this sound cannot be heard.
    //! Only sounds which loop and are attached can be slept.
    bool sound_renderer::should_be_sleeping( const SoundSrc& s ) const
    {
        // return false;

        return s.details.loop && s.attached
               && ( ( *fvolumecalc_ )( distance3( s.attached->location().xyz, listenerxyz_ ),
                                       s.details.radius * SOLOSNAKE_SOUND_RADIUS_REF_FACTOR
                                       * SOLOSNAKE_SOUND_RADIUS_EXPANSION,
                                       s.details.radius * SOLOSNAKE_SOUND_RADIUS_EXPANSION,
                                       1.0f ) * s.details.vol ) <= soundVolLowerLimit_;
    }

    //! Sets the velocity of the listener.
    void sound_renderer::set_listener_velocity( float x, float y, float z )
    {
        alListener3f( AL_VELOCITY, x, y, z );
    }

    //! Sets the listener's location and orientation.
    void sound_renderer::set_listener_orientation( const float* unaliased location,
            const float* unaliased upwards,
            const float* unaliased forwards )
    {
        listenerxyz_[0] = location[0];
        listenerxyz_[1] = location[1];
        listenerxyz_[2] = location[2];

        alListenerfv( AL_POSITION, location );

        const float orientate[]
            = { forwards[0], forwards[1], forwards[2], upwards[0], upwards[1], upwards[2] };

        alListenerfv( AL_ORIENTATION, orientate );
    }

    //! Tries to load the named buffer file via the internal file-finder. If the
    //! file cannot be located, or the file is not an accepted format, then
    //! an exception is thrown, else returns a handle which can be used to play
    //! this sound.
    sound_renderer::BufferHandle sound_renderer::load_buffer( const std::string& file )
    {
        SOLOSNAKE_ASSERT_OPENAL_OK;

        sound_renderer::BufferHandle h
            = alutCreateBufferFromFile( files_->get_file( file ).string().c_str() );

        if( h != AL_NONE )
        {
            buffers_.push_back( h );
        }
        else
        {
            ss_throw( "Unable to create sound buffer." );
        }

        SOLOSNAKE_ASSERT_OPENAL_OK;
        return h;
    }

    //! The default behaviour is to play the sound with no relational effects
    //! (it has no position) and without looping, at default volume. This is
    //! intended really for simple playing of sounds such as notifications and
    //! perhaps GUI elements etc.
    //! Returns true if the sounds is successfully played. If there are too many
    //! sounds already playing within earshot of the listener, then a sound may
    //! not be played.
    bool sound_renderer::play_buffer( sound_renderer::BufferHandle bh )
    {
        SOLOSNAKE_ASSERT_OPENAL_OK;
        assert( std::find( buffers_.cbegin(), buffers_.cend(), bh ) != buffers_.cend() );
        assert( alIsBuffer( bh ) );

        if( nplaying_ < sources_.size() )
        {
            assert( !is_playing( sources_[nplaying_].src ) );
            assert( sources_[nplaying_].attached.get() == nullptr );

            SoundSrc& s = sources_[nplaying_++];

            s.details.bh = bh;
            s.details.vol = 1.0f;
            s.details.radius = 0.0f;
            s.details.loop = false;

            // Attach source to buffer and play it:
            alSourcei( s.src, AL_BUFFER, bh );
            alSourcei( s.src, AL_SOURCE_RELATIVE, AL_FALSE );
            alSourcei( s.src, AL_LOOPING, AL_FALSE );
            alSourcef( s.src, AL_GAIN, 1.0f );
            alSourcef( s.src, AL_MIN_GAIN, 0.0f );
            alSourcef( s.src, AL_MAX_GAIN, 1.0f );

            alSourceRewind( s.src );
            alSourcePlay( s.src );

            SOLOSNAKE_ASSERT_OPENAL_OK;

            return true;
        }

        return false;
    }

    //! Starts playing a sound, and associates the sound with a world object. The
    //! sound will track the world object's position and velocity and update the
    //! sounds every 'update' until the object is invalid, or the sound has
    //! finished playing.
    //! Returns true if the sounds is successfully played. If there are too many
    //! sounds already playing within earshot of the listener, then a sound may
    //! not be played.
    //! A sound dies and is discarded when its world object becomes invalid.
    bool sound_renderer::play_buffer( sound_renderer::BufferHandle bh,
                                      std::shared_ptr<iworldobject> w,
                                      float volume,
                                      float radius,
                                      bool loop )
    {
        if( bh && volume > 0.0f && radius > 0.0f && w )
        {
            if( !w->is_valid() )
            {
                // If the object is invalid, it is as if the sound played and
                // was immediately stopped. We return true because false
                // indicates a failure, and this is actually a success condition.
                return true;
            }

            if( nplaying_ < sources_.size() )
            {
                SOLOSNAKE_ASSERT_OPENAL_OK;
                assert( std::find( buffers_.cbegin(), buffers_.cend(), bh ) != buffers_.cend() );
                assert( alIsBuffer( bh ) );
                assert( !is_playing( sources_[nplaying_].src ) );
                assert( sources_[nplaying_].attached.get() == nullptr );

                SoundSrc& s = sources_[nplaying_];

                s.details.bh = bh;
                s.details.vol = volume;
                s.details.radius = radius;
                s.details.loop = loop;
                s.attached = w;

                // Attach source to buffer and play it:
                alSourcei( s.src, AL_BUFFER, bh );
                alSourcei( s.src, AL_SOURCE_RELATIVE, AL_FALSE );
                alSourcei( s.src, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
                alSourcef( s.src, AL_MIN_GAIN, 0.0f );
                alSourcef( s.src, AL_MAX_GAIN, volume > 1.0f ? 1.0f : volume );
                alSourcef( s.src, AL_GAIN, volume > 1.0f ? 1.0f : volume );
                alSourcef( s.src, AL_REFERENCE_DISTANCE, radius * SOLOSNAKE_SOUND_RADIUS_REF_FACTOR );
                alSourcef( s.src, AL_MAX_DISTANCE, radius );
                alSourcefv( s.src, AL_POSITION, w->location().xyz );
                alSourcefv( s.src, AL_VELOCITY, w->velocity().xyz );

                // This needs to come before any sleep checks, to ensure the states
                // are correct for the tick loop.
                alSourceRewind( s.src );
                alSourcePlay( s.src );

                if( loop && should_be_sleeping( s ) )
                {
                    sleep_sound( s );
                }

                ++nplaying_;

                SOLOSNAKE_ASSERT_OPENAL_OK;

                return true;
            }
            else if( loop )
            {
                // We cannot play the sound right now, because we do not have
                // enough sources. But it is a looping sound so we can sleep it
                // and it will be resurrected when a slot is available.
                Sleeping sleeper;
                sleeper.snd.bh = bh;
                sleeper.snd.vol = volume;
                sleeper.snd.radius = radius;
                sleeper.snd.loop = loop;
                sleeper.attached = w;
                sleeping_.push_back( sleeper );

                return true;
            }
        }

        return false;
    }

    //! A potentially expensive call, it will check all the buffers to see
    //! if the named buffer is playing or pending in any of them. Sounds whose
    //! buffers AND objects match the parameters will be stopped.
    void sound_renderer::kill_sound( sound_renderer::BufferHandle bh, std::shared_ptr<iworldobject> w )
    {
        if( bh )
        {
            for( size_t i = 0; i < nplaying_; ++i )
            {
                if( sources_[i].details.bh == bh && sources_[i].attached == w )
                {
                    alSourceStop( sources_[i].src );
                }
            }

            // Remove also from sleepers, if its there.
            if( nplaying_ < sources_.size() && !sleeping_.empty() )
            {
                size_t n = sleeping_.size();
                size_t i = 0;
                while( n-- )
                {
                    Sleeping& sleeper = sleeping_[i];

                    if( sleeper.snd.bh == bh && sleeper.attached == w )
                    {
                        // Swap out removed sleeper.
                        sleeping_[i] = sleeping_[n];
                        sleeping_.resize( n );
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }
    }
}
