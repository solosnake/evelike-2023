#include "solosnake/blue/blue_fxsound.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_gamelogicconstants.hpp"

namespace blue
{
    fxsound::fxsound( const float y, const HeapOnly )
        : sound_volume_( BLUE_DEFAULT_SOUND_FX_VOLUME ), sound_radius_( BLUE_DEFAULT_SOUND_FX_RADIUS )
    {
        set_location( solosnake::point3d( 0.0f, y, 0.0f ) );
    }

    fxsound::~fxsound()
    {
    }

    void fxsound::reset( const float x, const float z )
    {
        const float y = location()[1];
        static_cast<solosnake::iworldobject&>( *this ) = solosnake::iworldobject();
        set_location( solosnake::point3d( x, y, z ) );
    }

    void fxsound::play_sound( solosnake::sound_renderer* const p, const solosnake::sound_renderer::BufferHandle h )
    {
        p->play_buffer( h, shared_from_this(), sound_volume_, sound_radius_, false );
    }

    std::shared_ptr<fxsound> fxsound::make_fxsound( const float y )
    {
        return std::make_shared<fxsound>( y, HeapOnly() );
    }
}
