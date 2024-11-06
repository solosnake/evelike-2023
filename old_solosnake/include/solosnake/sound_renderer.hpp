#ifndef solosnake_sound_renderer_hpp
#define solosnake_sound_renderer_hpp

#include <memory>
#include <string>
#include <vector>
#include "solosnake/matrix3d.hpp"
#include "solosnake/external/openal.hpp"

namespace solosnake
{
    class ifilefinder;
    class iworldobject;

    //! Plays sound effects. Sounds can be either played as if relative to a
    //! moving listener, or played without any attenuation. Position-relative
    //! sounds are associated with a world object, which they use to update
    //! their position and velocity.
    //! This class is non-copyable and it is recommended you create it as a
    //! smart pointer.
    class sound_renderer
    {
    public:
        enum FalloffModel
        {
            LinearFalloff,
            InverseFalloff,
            ExponentialFalloff
        };

        typedef ALuint BufferHandle;

        sound_renderer( std::shared_ptr<ifilefinder>, FalloffModel, float worldScale, size_t maxSounds );

        ~sound_renderer();

        BufferHandle load_buffer( const std::string& );

        bool play_buffer( BufferHandle );

        bool play_buffer(
            BufferHandle, std::shared_ptr<iworldobject>, float volume, float radius, bool loop );

        void kill_sound( BufferHandle, std::shared_ptr<iworldobject> );

        void set_listener_orientation( const float* unaliased location,
                                       const float* unaliased upwards,
                                       const float* unaliased forwards );

        void set_listener_velocity( float x, float y, float z );

        void update();

    private:
        sound_renderer( const sound_renderer& );

        sound_renderer& operator=( const sound_renderer& );

        struct Sound
        {
            Sound();
            BufferHandle bh;
            float vol;
            float radius;
            bool loop;
        };

        struct SoundSrc
        {
            SoundSrc();
            std::shared_ptr<iworldobject> attached;
            ALuint src;
            Sound details;
        };

        struct Sleeping
        {
            Sleeping();
            Sleeping( const Sound& s, std::shared_ptr<iworldobject> p ) : attached( p ), snd( s )
            {
            }
            std::shared_ptr<iworldobject> attached;
            Sound snd;
        };

        bool should_be_sleeping( const SoundSrc& ) const;

        bool should_be_awake( const Sleeping& ) const;

        void sleep_sound( const SoundSrc& );

        static float volume_in_linear( float, float, float, float );

        static float volume_in_inverse( float, float, float, float );

        static float volume_in_exponential( float, float, float, float );

    private:
        std::shared_ptr<ifilefinder> files_;
        std::vector<ALuint> buffers_;
        std::vector<SoundSrc> sources_;
        std::vector<Sleeping> sleeping_;
        size_t nplaying_;
        float listenerxyz_[3];
        float soundVolLowerLimit_;
        float( *fvolumecalc_ )( float, float, float, float );
    };
}

#endif
